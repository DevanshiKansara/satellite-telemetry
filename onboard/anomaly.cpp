#include "anomaly.hpp"

namespace tlm {

uint8_t AnomalyDetector::evaluate(const Telemetry& t) {
    uint8_t flags = 0;
    if (t.battery_mv < limits_.battery_min_mv) flags |= FLAG_LOW_BATTERY;
    if (t.temperature_cdeg > limits_.temp_max_cdeg) flags |= FLAG_OVER_TEMP;
    if (has_prev_ &&
        (t.temperature_cdeg - prev_temp_) > limits_.temp_rise_max_cdeg) {
        flags |= FLAG_TEMP_RISING_FAST;
    }
    prev_temp_ = t.temperature_cdeg;
    has_prev_ = true;
    return flags;
}

}  // namespace tlm
