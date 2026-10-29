#include <cstdio>
#include <iostream>

#include "packet.hpp"

int main() {
    std::cerr << "On-board system starting..." << std::endl;
    for (uint16_t seq = 0; seq < 5; ++seq) {
        tlm::Telemetry t{};
        t.battery_mv = static_cast<uint16_t>(7400 - seq * 10);
        t.temperature_cdeg = static_cast<int16_t>(2150 + seq * 25);
        t.solar_current_ma = 850;
        t.status_flags = 0x01;
        auto pkt = tlm::encode_packet(seq, seq * 1000u, t);
        for (uint8_t b : pkt) std::printf("%02X", b);
        std::printf("\n");
    }
    return 0;
}
