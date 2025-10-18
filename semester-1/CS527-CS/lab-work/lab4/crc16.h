#ifndef CRC16_H
#define CRC16_H

#include <stddef.h>
#include <stdint.h>

// Compute CRC-16/CCITT (poly 0x1021, init 0xFFFF) over 'len' bytes.
uint16_t crc16_ccitt(const uint8_t *data, size_t len);

#endif // CRC16_H
