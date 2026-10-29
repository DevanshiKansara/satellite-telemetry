#include "packet.hpp"

namespace tlm {

uint16_t crc16_ccitt(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x8000) {
                crc = static_cast<uint16_t>((crc << 1) ^ 0x1021);
            } else {
                crc = static_cast<uint16_t>(crc << 1);
            }
        }
    }
    return crc;
}

static void put_u16(std::vector<uint8_t>& v, uint16_t x) {
    v.push_back(static_cast<uint8_t>(x >> 8));
    v.push_back(static_cast<uint8_t>(x & 0xFF));
}

static void put_u32(std::vector<uint8_t>& v, uint32_t x) {
    put_u16(v, static_cast<uint16_t>(x >> 16));
    put_u16(v, static_cast<uint16_t>(x & 0xFFFF));
}

std::vector<uint8_t> encode_packet(uint16_t seq, uint32_t timestamp_ms,
                                   const Telemetry& t) {
    std::vector<uint8_t> p;
    p.reserve(19);
    p.push_back(SYNC0);
    p.push_back(SYNC1);
    put_u16(p, seq);
    put_u32(p, timestamp_ms);
    p.push_back(TYPE_HOUSEKEEPING);
    p.push_back(7);  // payload length
    put_u16(p, t.battery_mv);
    put_u16(p, static_cast<uint16_t>(t.temperature_cdeg));
    put_u16(p, t.solar_current_ma);
    p.push_back(t.status_flags);
    put_u16(p, crc16_ccitt(p.data(), p.size()));
    return p;
}

}  // namespace tlm
