#include "frame.h"          // frame_t and constants
#include "crc16.h"          // crc16_ccitt()
#include <stdio.h>           // printf

// Fill checksum with CRC-16 of payload bytes
void frame_compute_checksum(frame_t *f) {
  if (!f) return;                          // guard null
  f->checksum = crc16_ccitt(f->data, f->length); // CRC over data[0..len-1]
}

// Return 1 if checksum matches payload, else 0
int frame_validate_checksum(const frame_t *f) {
  if (!f) return 0;
  uint16_t expected = crc16_ccitt(f->data, f->length); // recompute CRC
  return expected == f->checksum;                      // compare with stored
}

// Clamp header fields to legal ranges
void frame_normalize_headers(frame_t *f) {
  if (!f) return;
  f->device_addr &= MAX_ADDR_VALUE; // keep 6 bits (0..63)
  f->packet_type  = f->packet_type ? 1 : 0;   // 0 or 1
  f->request_type = f->request_type ? 1 : 0;  // 0 or 1
}

// Short labels for logs
static const char* pkt_type_str(uint8_t t)  { return t ? "ACK"   : "DATA"; }
static const char* req_type_str(uint8_t rt) { return rt ? "WRITE" : "READ"; }

// Print one-line frame summary for logs
void frame_print_summary(const char* prefix, const frame_t *f) {
  if (!f) return;
  printf("%s dst=%02u type=%s req=%s mem=0x%04X len=%u crc=0x%04X\n",
         prefix ? prefix : "",                               // leading label
         (unsigned)(f->device_addr & MAX_ADDR_VALUE),        // 6-bit addr
         pkt_type_str(f->packet_type),                       // DATA/ACK
         req_type_str(f->request_type),                      // READ/WRITE
         f->mem_addr, f->length, f->checksum);               // payload info
}
