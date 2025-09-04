
#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include "constants.h"

typedef enum
{
  PKT_DATA = 0,
  PKT_ACK = 1
} packet_type_t;

typedef enum
{
  REQ_READ = 0,
  REQ_WRITE = 1
} request_type_t;

typedef struct
{
  uint8_t device_addr;
  uint8_t packet_type;
  uint8_t request_type;
  uint32_t mem_addr;
  uint8_t length;
  uint8_t data[MAX_DATA_LEN];
  uint16_t checksum;
} frame_t;

void frame_compute_checksum(frame_t *f);
int frame_validate_checksum(const frame_t *f);

#endif
