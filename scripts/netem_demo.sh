#!/usr/bin/env bash
# Usage: scripts/netem_demo.sh [loss_percent] [packet_count]
set -euo pipefail
cd "$(dirname "$0")/.."

LOSS="${1:-20}"
COUNT="${2:-150}"
PORT=5005
GROUND_PID=""
OUT=/tmp/ground_netem_out.txt

cleanup() {
    sudo tc qdisc del dev lo root 2>/dev/null || true
    [ -n "$GROUND_PID" ] && kill "$GROUND_PID" 2>/dev/null || true
}
trap cleanup EXIT

if systemctl is-active --quiet satellite-onboard; then
    echo "Stop the service first: sudo systemctl stop satellite-onboard"
    exit 1
fi

python3 ground/receiver.py --idle-timeout 3 --log /tmp/ground_netem.log > "$OUT" &
GROUND_PID=$!
sleep 1

echo "== Injecting ${LOSS}% loss + 100 ms delay on lo =="
sudo tc qdisc add dev lo root netem loss "${LOSS}%" delay 100ms

echo "== Sending ${COUNT} packets =="
./build/onboard 127.0.0.1 "$PORT" "$COUNT" 100 2>/dev/null

wait "$GROUND_PID" || true
GROUND_PID=""

SUMMARY=$(grep "stopped" "$OUT")
echo "$SUMMARY"
RECEIVED=$(echo "$SUMMARY" | sed -E 's/.*received=([0-9]+).*/\1/')
LOST=$(echo "$SUMMARY" | sed -E 's/.*lost=([0-9]+).*/\1/')
echo "sent=${COUNT} received=${RECEIVED} detected_lost=${LOST}"
echo "undetected tail loss: $((COUNT - RECEIVED - LOST)) packet(s)"
