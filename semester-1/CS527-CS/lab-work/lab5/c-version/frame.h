#pragma once
#include <stdint.h>
#include "constants.h"

/* Packet structure (Lab-4 protocol). For Lab-5 we route by 32-bit mem_addr. */
typedef struct frame_t {
  uint8_t        device_addr;     /* 0..63 (kept for Lab-4 compatibility; not used by bus) */
  packet_type_t  packet_type;     /* DATA or ACK */
  request_type_t request_type;    /* READ or WRITE */
  uint32_t       mem_addr;        /* absolute memory-mapped address */
  uint8_t        length;          /* 0..255 (covers only DATA bytes) */
  uint8_t        data[FRAME_DATA_MAX];
  uint16_t       checksum;        /* CRC16 of 'data[0..length-1]' */
} frame_t;

/* Helpers */
void     frame_compute_checksum(frame_t* f);
int      frame_validate_checksum(const frame_t* f); /* 1 valid, 0 invalid */
