#include "udp.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

namespace net {

UdpSender::UdpSender(const std::string& host, uint16_t port) {
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) throw std::runtime_error("socket() failed");
    std::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &addr_.sin_addr) != 1) {
        ::close(fd_);
        throw std::runtime_error("invalid IPv4 address: " + host);
    }
}

UdpSender::~UdpSender() {
    if (fd_ >= 0) ::close(fd_);
}

bool UdpSender::send(const std::vector<uint8_t>& data) {
    ssize_t n = ::sendto(fd_, data.data(), data.size(), 0,
                         reinterpret_cast<const sockaddr*>(&addr_),
                         sizeof(addr_));
    return n == static_cast<ssize_t>(data.size());
}

}  // namespace net
