import pathlib
import socket
import subprocess
import sys

import pytest

ROOT = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "ground"))
from decoder import decode_packet  # noqa: E402

ONBOARD = ROOT / "build" / "onboard"


@pytest.mark.skipif(not ONBOARD.exists(), reason="build the project first")
def test_onboard_to_ground_link():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", 0))  # free port chosen by the OS
    sock.settimeout(5)
    port = sock.getsockname()[1]

    proc = subprocess.Popen(
        [str(ONBOARD), "127.0.0.1", str(port), "10", "20"],
        stderr=subprocess.DEVNULL,
    )
    packets = []
    try:
        for _ in range(10):
            raw, _addr = sock.recvfrom(1024)
            packets.append(decode_packet(raw))
    finally:
        proc.wait(timeout=10)
        sock.close()

    assert [p["seq"] for p in packets] == list(range(10))
    assert all(p["status_flags"] == 0 for p in packets)
