class SequenceTracker:
    """Tracks 16-bit packet sequence numbers and counts losses."""

    def __init__(self) -> None:
        self.expected = None
        self.received = 0
        self.lost = 0
        self.out_of_order = 0

    def update(self, seq: int) -> int:
        """Register a received sequence number; return packets missed before it."""
        self.received += 1
        if self.expected is None:
            self.expected = (seq + 1) & 0xFFFF
            return 0
        diff = (seq - self.expected) & 0xFFFF
        if diff == 0:
            self.expected = (seq + 1) & 0xFFFF
            return 0
        if diff < 0x8000:  # packets were skipped
            self.lost += diff
            self.expected = (seq + 1) & 0xFFFF
            return diff
        self.out_of_order += 1  # duplicate or late packet
        return 0
