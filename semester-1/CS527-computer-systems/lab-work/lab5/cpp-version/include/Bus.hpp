#pragma once
#include <vector>
#include <memory>
#include "Frame.hpp"
#include "Device.hpp"

class Bus {
public:
    Bus();
    void init();
    // 1=ACK, 0=NACK
    int send(const Frame& req, Frame& resp);

private:
    std::vector<std::unique_ptr<Device>> devices_;
    int route(std::uint32_t addr) const;
};
