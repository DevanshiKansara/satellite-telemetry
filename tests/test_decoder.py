import pathlib
import struct
import sys

import pytest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1] / "ground"))
from decoder import crc16_ccitt, decode_packet  # noqa: E402


def make_packet(seq=1, ts=1000, batt=7400, temp=2150, solar=850, flags=1):
    body = struct.pack(">BBHIBB", 0xA5, 0x5A, seq, ts, 1, 7)
    body += struct.pack(">HhHB", batt, temp, solar, flags)
    return body + struct.pack(">H", crc16_ccitt(body))


def test_crc_standard_check_value():
    assert crc16_ccitt(b"123456789") == 0x29B1


def test_valid_packet_decodes():
    pkt = decode_packet(make_packet())
    assert pkt["seq"] == 1
    assert pkt["battery_V"] == 7.4
    assert pkt["temperature_C"] == 21.5
    assert pkt["status_flags"] == 1


def test_negative_temperature():
    assert decode_packet(make_packet(temp=-500))["temperature_C"] == -5.0


def test_too_short():
    with pytest.raises(ValueError, match="too short"):
        decode_packet(make_packet()[:5])


def test_bad_sync():
    raw = bytearray(make_packet())
    raw[0] = 0x00
    with pytest.raises(ValueError, match="bad sync"):
        decode_packet(bytes(raw))


def test_length_mismatch():
    with pytest.raises(ValueError, match="length mismatch"):
        decode_packet(make_packet() + b"\x00")


@pytest.mark.parametrize("position", range(19))
def test_any_corrupted_byte_is_rejected(position):
    raw = bytearray(make_packet())
    raw[position] ^= 0xFF
    with pytest.raises(ValueError):
        decode_packet(bytes(raw))
