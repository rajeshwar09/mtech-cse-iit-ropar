#ifndef CRC_H
#define CRC_H
#include <stdint.h>
#include <stddef.h>
#include "constants.h"
static inline uint16_t crc16_ccitt(const uint8_t *p, size_t n, uint16_t s, uint16_t poly)
{
  uint16_t c = s;
  for (size_t i = 0; i < n; i++)
  {
    c ^= (uint16_t)p[i] << 8;
    for (int b = 0; b < 8; b++)
    {
      c = (c & 0x8000) ? (uint16_t)((c << 1) ^ poly) : (uint16_t)(c << 1);
    }
  }
  return c;
}
static inline uint16_t crc_payload(const uint8_t *p, size_t n) { return crc16_ccitt(p, n, CRC16_INIT, CRC16_POLY); }
#endif
