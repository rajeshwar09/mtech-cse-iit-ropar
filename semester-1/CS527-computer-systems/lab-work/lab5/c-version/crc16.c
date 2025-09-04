
#include "crc16.h"
#include "constants.h"

uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
  uint16_t crc = CRC16_INIT;
  for (size_t i = 0; i < len; ++i)
  {
    crc ^= (uint16_t)(data[i] << 8);
    for (int b = 0; b < 8; ++b)
    {
      if (crc & 0x8000)
        crc = (uint16_t)((crc << 1) ^ CRC16_POLY);
      else
        crc = (uint16_t)(crc << 1);
    }
  }
  return crc;
}
