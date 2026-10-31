#include <gtest/gtest.h>

#include "anomaly.hpp"
#include "packet.hpp"

TEST(Crc, StandardCheckValue) {
    const uint8_t data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    EXPECT_EQ(tlm::crc16_ccitt(data, sizeof(data)), 0x29B1);
}

TEST(Packet, LayoutAndLength) {
    tlm::Telemetry t{7400, 2150, 850, 0x01};
    auto p = tlm::encode_packet(1, 1000, t);
    ASSERT_EQ(p.size(), 19u);
    EXPECT_EQ(p[0], 0xA5);
    EXPECT_EQ(p[1], 0x5A);
    EXPECT_EQ(p[3], 1);   // sequence low byte
    EXPECT_EQ(p[9], 7);   // payload length
}

TEST(Packet, CrcCoversAllPreviousBytes) {
    tlm::Telemetry t{7400, 2150, 850, 0x01};
    auto p = tlm::encode_packet(5, 2000, t);
    uint16_t crc_in_packet = static_cast<uint16_t>((p[17] << 8) | p[18]);
    EXPECT_EQ(crc_in_packet, tlm::crc16_ccitt(p.data(), p.size() - 2));
}

TEST(Anomaly, NominalHasNoFlags) {
    tlm::AnomalyDetector d;
    tlm::Telemetry t{7400, 2150, 850, 0};
    EXPECT_EQ(d.evaluate(t), 0);
}

TEST(Anomaly, LowBattery) {
    tlm::AnomalyDetector d;
    tlm::Telemetry t{6700, 2150, 850, 0};
    EXPECT_EQ(d.evaluate(t), tlm::FLAG_LOW_BATTERY);
}

TEST(Anomaly, OverTemperature) {
    tlm::AnomalyDetector d;
    tlm::Telemetry t{7400, 4100, 850, 0};
    EXPECT_EQ(d.evaluate(t), tlm::FLAG_OVER_TEMP);
}

TEST(Anomaly, FastTemperatureRise) {
    tlm::AnomalyDetector d;
    tlm::Telemetry first{7400, 2150, 850, 0};
    tlm::Telemetry second{7400, 2300, 850, 0};  // +1.50 C in one sample
    EXPECT_EQ(d.evaluate(first), 0);
    EXPECT_EQ(d.evaluate(second), tlm::FLAG_TEMP_RISING_FAST);
}
