#pragma once
#include <cstdint>

#include "packet.hpp"

namespace tlm {

// status_flags bits (0 = nominal)
constexpr uint8_t FLAG_LOW_BATTERY = 0x02;
constexpr uint8_t FLAG_OVER_TEMP = 0x04;
constexpr uint8_t FLAG_TEMP_RISING_FAST = 0x08;

struct Limits {
    uint16_t battery_min_mv = 6800;
    int16_t temp_max_cdeg = 4000;        // 40.00 C
    int16_t temp_rise_max_cdeg = 100;    // max rise per sample, 1.00 C
};

class AnomalyDetector {
public:
    explicit AnomalyDetector(Limits limits = Limits{}) : limits_(limits) {}
    uint8_t evaluate(const Telemetry& t);

private:
    Limits limits_;
    bool has_prev_ = false;
    int16_t prev_temp_ = 0;
};

}  // namespace tlm
