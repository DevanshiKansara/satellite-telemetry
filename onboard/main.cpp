#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include "anomaly.hpp"
#include "packet.hpp"
#include "udp.hpp"

// usage: onboard [host] [port] [count] [interval_ms]
int main(int argc, char** argv) {
    const std::string host = argc > 1 ? argv[1] : "127.0.0.1";
    const uint16_t port =
        argc > 2 ? static_cast<uint16_t>(std::atoi(argv[2])) : 5005;
    const int count = argc > 3 ? std::atoi(argv[3]) : 60;
    const int interval_ms = argc > 4 ? std::atoi(argv[4]) : 200;

    try {
        net::UdpSender link(host, port);
        tlm::AnomalyDetector detector;
        std::cerr << "[onboard] sending " << count << " packets to " << host
                  << ":" << port << std::endl;

        for (int i = 0; i < count; ++i) {
            int batt = 7400 - i * 5;
            if (i >= 40) batt -= (i - 40) * 60;
            int temp = 2150 + i * 5;
            if (i >= 25) temp += (i - 25) * 150;

            tlm::Telemetry t{};
            t.battery_mv = static_cast<uint16_t>(batt < 0 ? 0 : batt);
            t.temperature_cdeg = static_cast<int16_t>(temp);
            t.solar_current_ma = 850;
            t.status_flags = detector.evaluate(t);

            if (t.status_flags != 0) {
                std::fprintf(stderr, "[onboard] seq=%d anomaly flags=0x%02X\n",
                             i, t.status_flags);
            }

            auto pkt = tlm::encode_packet(static_cast<uint16_t>(i),
                                          static_cast<uint32_t>(i) * interval_ms,
                                          t);
            if (!link.send(pkt)) std::cerr << "[onboard] send failed\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }
    } catch (const std::exception& e) {
        std::cerr << "[onboard] error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
