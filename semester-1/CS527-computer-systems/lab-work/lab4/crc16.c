#include "crc16.h"        // declares crc16_ccitt()
#include "constants.h"    // provides CRC16_INIT and CRC16_POLY

// Compute CRC-16/CCITT-FALSE over 'data[0..len-1]'
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
  uint16_t crc = CRC16_INIT;                 // start value (0xFFFF)

  for (size_t i = 0; i < len; i++) {         // process each byte
    crc ^= ((uint16_t)data[i]) << 8;         // mix byte into high 8 bits

    for (int b = 0; b < 8; ++b) {            // for each bit in the byte
      if (crc & 0x8000) {                    // if MSB is 1
        crc = (crc << 1) ^ CRC16_POLY;       // shift and xor with polynomial (0x1021) x^16 + x^12 + x^5 + 1
      } else {                               // if MSB is 0
        crc <<= 1;                           // just shift left
      }
    }
  }

  return crc;                                // final 16-bit CRC
}