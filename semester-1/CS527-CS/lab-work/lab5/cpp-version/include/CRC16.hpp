#pragma once
#include <cstdint>
#include <cstddef>

// CRC-16/CCITT-FALSE over data bytes only
std::uint16_t crc16_ccitt_false(const std::uint8_t* data, std::size_t len);
