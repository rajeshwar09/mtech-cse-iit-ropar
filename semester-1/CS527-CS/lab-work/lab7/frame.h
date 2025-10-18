#ifndef FRAME_H
#define FRAME_H
#include <stdint.h>
#include "constants.h"
typedef struct
{
  uint8_t device_id;
  uint8_t packet_type;
  uint8_t rw_flag;
  uint32_t start_addr;
  uint32_t length;
  uint8_t payload[STORAGE_BYTES];
  uint16_t crc;
} Frame;
#endif
