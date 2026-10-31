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

// usage: onboard [host] [port] [count (0 = run forever)] [interval_ms]
int main(int argc, char** argv) {
    const std::string host = argc > 1 ? argv[1] : "127.0.0.1";
    const uint16_t port =
        argc > 2 ? static_cast<uint16_t>(std::atoi(argv[2])) : 5005;
    const int count = argc > 3 ? std::atoi(argv[3]) : 60;
    const int interval_ms = argc > 4 ? std::atoi(argv[4]) : 200;

    try {
        net::UdpSender link(host, port);
        tlm::AnomalyDetector detector;
        std::cerr << "[onboard] sending to " << host << ":" << port
                  << " (count=" << count << ", interval=" << interval_ms
                  << " ms)" << std::endl;

        for (long i = 0; count <= 0 || i < count; ++i) {
            const int phase = static_cast<int>(i % 100);  // scenario repeats
            int batt = 7400 - phase * 5;
            if (phase >= 40) batt -= (phase - 40) * 60;
            int temp = 2150 + phase * 5;
            if (phase >= 25) temp += (phase - 25) * 150;
            if (temp > 8500) temp = 8500;
            if (batt < 5000) batt = 5000;

            tlm::Telemetry t{};
            t.battery_mv = static_cast<uint16_t>(batt < 0 ? 0 : batt);
            t.temperature_cdeg = static_cast<int16_t>(temp);
            t.solar_current_ma = 850;
            t.status_flags = detector.evaluate(t);

            if (t.status_flags != 0) {
                std::fprintf(stderr, "[onboard] seq=%ld anomaly flags=0x%02X\n",
                             i, t.status_flags);
            }

            auto pkt = tlm::encode_packet(
                static_cast<uint16_t>(i & 0xFFFF),
                static_cast<uint32_t>(i * interval_ms), t);
            if (!link.send(pkt)) std::cerr << "[onboard] send failed\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }
    } catch (const std::exception& e) {
        std::cerr << "[onboard] error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
