import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1] / "ground"))
from gaps import SequenceTracker  # noqa: E402


def feed(seqs):
    t = SequenceTracker()
    gaps = [t.update(s) for s in seqs]
    return t, gaps


def test_no_loss():
    t, gaps = feed(range(10))
    assert (t.received, t.lost, t.out_of_order) == (10, 0, 0)
    assert gaps == [0] * 10


def test_single_gap():
    t, gaps = feed([0, 1, 3])
    assert t.lost == 1
    assert gaps == [0, 0, 1]


def test_multiple_and_long_gaps():
    t, _ = feed([0, 2, 3, 7])
    assert t.lost == 1 + 3


def test_wraparound_without_loss():
    t, _ = feed([65534, 65535, 0, 1])
    assert t.lost == 0


def test_wraparound_with_loss():
    t, _ = feed([65534, 1])
    assert t.lost == 2  # 65535 and 0 were missed


def test_duplicate_is_not_loss():
    t, _ = feed([5, 5, 6])
    assert t.lost == 0
    assert t.out_of_order == 1


def test_late_packet_is_not_huge_loss():
    t, _ = feed([5, 6, 7, 6, 8])
    assert t.lost == 0
    assert t.out_of_order == 1
