# Runbook 2: Anomaly response

**Purpose:** react correctly when the ground station reports an `ALERT`.
**Verified:** 2026-10-31 using the built-in anomaly scenario.

## Flags

| Flag | Meaning | Check | Response |
|---|---|---|---|
| `LOW_BATTERY` | battery below 6.8 V | is the value still falling? | confirm trend over the next packets; reduce load (simulated) |
| `OVER_TEMP` | temperature above 40 °C | does it keep rising? | confirm trend; switch to safe mode (simulated) |
| `TEMP_RISING_FAST` | more than 1 °C rise between packets | combined with `OVER_TEMP`? | treat as precursor of over-temperature |

## Steps
1. Note the first alert: `grep ALERT ground_events.log | head -3`
2. Check for gaps around it: `grep GAP ground_events.log | tail`. A lost packet may have carried the first alert.
3. Follow the trend for at least 5 packets before acting.
4. Record the event (time, flags, values) and the action taken.

## Expected result
- Alerts clear (flags 0) once the values return to nominal limits.
