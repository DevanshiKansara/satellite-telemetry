# Runbook 1: Nominal start-up and verification

**Purpose:** bring up the telemetry chain and confirm it is healthy.
**Verified:** 2026-10-31, Ubuntu 24.04 (WSL2).

## Preconditions
- Project built (`cmake --build build`) and all tests pass (`./scripts/run_tests.sh`)
- No traffic shaping active: `tc qdisc show dev lo` shows no `netem`

## Steps
1. Start the ground station: `python3 ground/receiver.py`
2. Start the on-board service: `sudo systemctl start satellite-onboard`
3. Check the service: `systemctl status satellite-onboard --no-pager`

## Expected result
- Service state `active (running)`, process user `satellite` (`ps -o user,pid,cmd -C onboard`)
- Ground shows `OK` lines about once per second with increasing `seq`
- No `GAP` or `DROPPED` lines in the first minute

## If it fails
- No packets: check the port (5005) and that the ground station was started first
- Service not running: `journalctl -u satellite-onboard -n 20 --no-pager`
