#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace tlm {

constexpr uint8_t SYNC0 = 0xA5;
constexpr uint8_t SYNC1 = 0x5A;
constexpr uint8_t TYPE_HOUSEKEEPING = 0x01;

struct Telemetry {
    uint16_t battery_mv;
    int16_t temperature_cdeg;  // 0.01 degrees C
    uint16_t solar_current_ma;
    uint8_t status_flags;
};

uint16_t crc16_ccitt(const uint8_t* data, size_t len);
std::vector<uint8_t> encode_packet(uint16_t seq, uint32_t timestamp_ms,
                                   const Telemetry& t);

}  // namespace tlm
