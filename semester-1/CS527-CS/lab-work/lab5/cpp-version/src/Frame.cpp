#include "Frame.hpp"
#include "CRC16.hpp"

void Frame::computeChecksum() {
    checksum = crc16_ccitt_false(data.data(), length);
}

bool Frame::validateChecksum() const {
    return crc16_ccitt_false(data.data(), length) == checksum;
}
