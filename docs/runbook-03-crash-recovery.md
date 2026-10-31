# Runbook 3: Crash recovery

**Purpose:** verify that the on-board process recovers from a crash.
**Verified:** 2026-10-31, Ubuntu 24.04 (WSL2).

## Steps
1. Note the process ID: `systemctl show satellite-onboard -p MainPID`
2. Simulate the crash: `sudo systemctl kill -s SIGKILL satellite-onboard`
3. Wait 4 seconds, then read the PID again.
4. Read the journal: `journalctl -u satellite-onboard -n 8 --no-pager`

## Expected result
- The PID changed (systemd restarted the service after about 2 s, `RestartSec=2`)
- The journal shows the kill, `Failed with result 'signal'` and `Scheduled restart job`
- Ground shows a `GAP` of the packets missed during the restart; telemetry continues afterwards

## Afterwards
- Stop the service if the test is over: `sudo systemctl stop satellite-onboard`
