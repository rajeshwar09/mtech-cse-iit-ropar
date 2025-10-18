#include "CRC16.hpp"

std::uint16_t crc16_ccitt_false(const std::uint8_t* data, std::size_t len) {
    std::uint16_t crc = 0xFFFFu; // init
    for (std::size_t i = 0; i < len; ++i) {
        crc ^= static_cast<std::uint16_t>(data[i]) << 8;
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x8000u) crc = static_cast<std::uint16_t>((crc << 1) ^ 0x1021u);
            else               crc = static_cast<std::uint16_t>(crc << 1);
        }
    }
    return crc;
}
