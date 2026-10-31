import argparse
import logging
import socket

from decoder import decode_packet

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
    args = ap.parse_args()

    logging.basicConfig(
        filename=args.log,
        level=logging.INFO,
        format="%(asctime)s %(levelname)s %(message)s",
    )

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((args.host, args.port))
    print(f"[ground] listening on {args.host}:{args.port} (Ctrl+C to stop)")

    expected_seq = None
    received = dropped = lost = 0
    try:
        while True:
            raw, _ = sock.recvfrom(1024)
            try:
                pkt = decode_packet(raw)
            except ValueError as err:
                dropped += 1
                print(f"DROPPED: {err}")
                logging.warning("DROPPED: %s", err)
                continue

            if expected_seq is not None and pkt["seq"] != expected_seq:
                gap = (pkt["seq"] - expected_seq) & 0xFFFF
                lost += gap
                print(f"GAP: {gap} packet(s) missing before seq {pkt['seq']}")
                logging.warning("GAP: %d packet(s) missing before seq %d",
                                gap, pkt["seq"])
            expected_seq = (pkt["seq"] + 1) & 0xFFFF
            received += 1

            line = (f"seq={pkt['seq']:4d} t={pkt['timestamp_ms']:6d}ms "
                    f"batt={pkt['battery_V']:.2f}V "
                    f"temp={pkt['temperature_C']:.2f}C")
            names = flag_names(pkt["status_flags"])
            if names:
                print(f"ALERT {line} {','.join(names)}")
                logging.error("ALERT %s %s", line, ",".join(names))
            else:
                print(f"OK    {line}")
                logging.info("OK %s", line)
    except KeyboardInterrupt:
        print(f"\n[ground] stopped. received={received} "
              f"dropped={dropped} lost={lost}")
        logging.info("stopped received=%d dropped=%d lost=%d",
                     received, dropped, lost)


if __name__ == "__main__":
    main()
