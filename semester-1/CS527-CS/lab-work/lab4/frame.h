#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <stddef.h>
#include "constants.h"

typedef enum {
    PKT_DATA = 0,   // master -> slave
    PKT_ACK  = 1    // slave  -> master (ACK or NACK or READ-DATA)
} packet_type_t;

typedef enum {
    REQ_READ  = 0,
    REQ_WRITE = 1
} request_type_t;

// A frame as specified in Lab 4 (fields are masked to spec widths where needed).
typedef struct frame_s {
    uint8_t  device_addr;   // 6-bit: 0..63 (0 reserved for master)
    uint8_t  packet_type;   // 0=data, 1=ack
    uint8_t  request_type;  // 0=read, 1=write (DC for simple ACK/NACK)
    uint16_t mem_addr;      // 16-bit memory address
    uint8_t  length;        // 0..255 data length
    uint8_t  data[MAX_DATA_LEN]; // data bytes (length valid)
    uint16_t checksum;      // CRC16 over 'data[0..length-1]' only
} frame_t;

// Helpers to (re)compute checksum and validate data integrity.
void frame_compute_checksum(frame_t *f);
int  frame_validate_checksum(const frame_t *f);

// Clamp/normalize headers to legal ranges (6-bit addr, etc.)
void frame_normalize_headers(frame_t *f);

// Pretty printer for one-line summaries
void frame_print_summary(const char* prefix, const frame_t *f);

#endif // FRAME_H
