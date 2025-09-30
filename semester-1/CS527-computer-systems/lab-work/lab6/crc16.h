#ifndef CRC16_H
#define CRC16_H

#include <stddef.h>
#include <stdint.h>
#include "constants.h"

static inline uint16_t crc16_ccitt(const uint8_t* data, size_t len,
                                   uint16_t seed, uint16_t poly) {
    uint16_t crc = seed;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x8000) crc = (uint16_t)((crc << 1) ^ poly);
            else              crc = (uint16_t)(crc << 1);
        }
    }
    return crc;
}

static inline uint16_t crc16_payload(const uint8_t* data, size_t len) {
    return crc16_ccitt(data, len, CRC16_INIT, CRC16_POLY);
}

#endif // CRC16_H