#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include "constants.h"

// Frame format (Lab-4 style; adapted):
// - device_id   : 5-bit value (stored in uint8_t, 0..31)
// - packet_type : 2-bit (DATA=00, ACKNAK=01, PRINT=10)
// - rw_flag     : 1-bit (0=READ, 1=WRITE). Only valid when packet_type==DATA
// - start_addr  : byte offset into device memory (0..2047)
// - length      : byte count for payload (0..2048)
// - payload     : up to STORAGE_BYTES bytes
// - crc         : CRC16 over payload[0..length-1]
typedef struct Frame {
    uint8_t  device_id;
    uint8_t  packet_type;
    uint8_t  rw_flag;
    uint32_t start_addr;
    uint32_t length;
    uint8_t  payload[STORAGE_BYTES];
    uint16_t crc;
} Frame;

#endif // FRAME_H