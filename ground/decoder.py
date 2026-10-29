import struct
import sys


def crc16_ccitt(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def decode_packet(raw: bytes) -> dict:
    if len(raw) < 12:
        raise ValueError("packet too short")
    if raw[0:2] != b"\xA5\x5A":
        raise ValueError("bad sync marker")
    seq, ts_ms, ptype, plen = struct.unpack(">HIBB", raw[2:10])
    if len(raw) != 10 + plen + 2:
        raise ValueError("length mismatch")
    crc_rx = struct.unpack(">H", raw[-2:])[0]
    crc_calc = crc16_ccitt(raw[:-2])
    if crc_rx != crc_calc:
        raise ValueError(f"CRC mismatch (rx {crc_rx:04X}, calc {crc_calc:04X})")
    battery_mv, temp_cdeg, solar_ma, flags = struct.unpack(
        ">HhHB", raw[10:10 + plen]
    )
    return {
        "seq": seq,
        "timestamp_ms": ts_ms,
        "type": ptype,
        "battery_V": battery_mv / 1000,
        "temperature_C": temp_cdeg / 100,
        "solar_current_mA": solar_ma,
        "status_flags": flags,
    }


def main() -> None:
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            print(decode_packet(bytes.fromhex(line)))
        except ValueError as err:
            print(f"DROPPED: {err}")


if __name__ == "__main__":
    main()
