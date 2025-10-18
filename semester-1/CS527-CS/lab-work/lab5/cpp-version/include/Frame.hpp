#pragma once
#include <cstdint>
#include <cstddef>
#include <array>
#include "Constants.hpp"

struct Frame {
    std::uint8_t  device_addr = 0;           // kept for Lab-4 compatibility; unused by bus
    PacketType    packet_type = PacketType::DATA;
    RequestType   request_type = RequestType::READ;
    std::uint32_t mem_addr    = 0;           // absolute MMIO address
    std::uint8_t  length      = 0;           // 0..255
    std::array<std::uint8_t, FRAME_DATA_MAX> data{};
    std::uint16_t checksum    = 0;           // CRC16 over data[0..length-1]

    void computeChecksum();
    bool validateChecksum() const;
};
