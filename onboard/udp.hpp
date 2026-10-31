#pragma once
#include <netinet/in.h>

#include <cstdint>
#include <string>
#include <vector>

namespace net {

class UdpSender {
public:
    UdpSender(const std::string& host, uint16_t port);
    ~UdpSender();
    UdpSender(const UdpSender&) = delete;
    UdpSender& operator=(const UdpSender&) = delete;

    bool send(const std::vector<uint8_t>& data);

private:
    int fd_ = -1;
    sockaddr_in addr_{};
};

}  // namespace net
