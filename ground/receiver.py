import argparse
import logging
import socket

from decoder import decode_packet
from gaps import SequenceTracker

FLAG_NAMES = {
    0x02: "LOW_BATTERY",
    0x04: "OVER_TEMP",
    0x08: "TEMP_RISING_FAST",
}


def flag_names(flags: int) -> list:
    return [name for bit, name in FLAG_NAMES.items() if flags & bit]


def main() -> None:
    ap = argparse.ArgumentParser(description="Ground segment UDP receiver")
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=5005)
    ap.add_argument("--log", default="ground_events.log")
    ap.add_argument("--idle-timeout", type=float, default=0.0,
                    help="stop after this many seconds without packets (0 = never)")
    args = ap.parse_args()

    logging.basicConfig(
        filename=args.log,
        level=logging.INFO,
        format="%(asctime)s %(levelname)s %(message)s",
    )

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((args.host, args.port))
    if args.idle_timeout > 0:
        sock.settimeout(args.idle_timeout)
    print(f"[ground] listening on {args.host}:{args.port} (Ctrl+C to stop)",
          flush=True)

    tracker = SequenceTracker()
    dropped = 0
    try:
        while True:
            try:
                raw, _ = sock.recvfrom(1024)
            except socket.timeout:
                print("[ground] idle timeout", flush=True)
                break
            try:
                pkt = decode_packet(raw)
            except ValueError as err:
                dropped += 1
                print(f"DROPPED: {err}", flush=True)
                logging.warning("DROPPED: %s", err)
                continue

            gap = tracker.update(pkt["seq"])
            if gap:
                print(f"GAP: {gap} packet(s) missing before seq {pkt['seq']}",
                      flush=True)
                logging.warning("GAP: %d packet(s) missing before seq %d",
                                gap, pkt["seq"])

            line = (f"seq={pkt['seq']:4d} t={pkt['timestamp_ms']:6d}ms "
                    f"batt={pkt['battery_V']:.2f}V "
                    f"temp={pkt['temperature_C']:.2f}C")
            names = flag_names(pkt["status_flags"])
            if names:
                print(f"ALERT {line} {','.join(names)}", flush=True)
                logging.error("ALERT %s %s", line, ",".join(names))
            else:
                print(f"OK    {line}", flush=True)
                logging.info("OK %s", line)
    except KeyboardInterrupt:
        print()

    summary = (f"received={tracker.received} dropped={dropped} "
               f"lost={tracker.lost} out_of_order={tracker.out_of_order}")
    print(f"[ground] stopped. {summary}", flush=True)
    logging.info("stopped %s", summary)


if __name__ == "__main__":
    main()
