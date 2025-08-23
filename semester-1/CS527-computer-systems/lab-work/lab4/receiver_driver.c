#include <stdio.h>
#include <string.h>
#include "receiver.h"
#include "logger.h"

static int is_range_valid(uint16_t start, uint8_t len) {
  if (len == 0) return 1;
  uint32_t end = (uint32_t)start + (uint32_t)len; // end exclusive
  return end <= SLAVE_MEM_SIZE;
}

void receiver_init(Receiver *r, uint8_t address) {
  if (!r) return;
  r->addr = (address & MAX_ADDR_VALUE);
  memset(r->memory, 0, sizeof(r->memory));
}

int receiver_process(Receiver *r, const frame_t *req, frame_t *resp) {
  if (!r || !req || !resp) return 0;

  // Initialize resp as an ACK/NACK frame addressed to master by default
  memset(resp, 0, sizeof(*resp));
  resp->device_addr = MASTER_ADDR;
  resp->packet_type = PKT_ACK;
  resp->request_type = req->request_type;

#if LOG_INFO
  LOGI("RX", "DEV %02u: got %s frame: req=%s addr=0x%04X len=%u\n",
        r->addr,
        req->packet_type ? "ACK(?)" : "DATA",
        req->request_type ? "WRITE" : "READ",
        req->mem_addr, req->length);
#endif

  // Only accept DATA frames from master addressed to this receiver
  if (req->packet_type != PKT_DATA) {
#if LOG_INFO
  LOGE("RX", "DEV %02u: NACK - unexpected packet type (not DATA)\n", r->addr);
#endif
    return 0;
  }
  if ((req->device_addr & MAX_ADDR_VALUE) != r->addr) {
#if LOG_INFO
  LOGE("RX", "DEV %02u: NACK - wrong destination (got %u)\n", r->addr, req->device_addr & MAX_ADDR_VALUE);
#endif
    return 0;
  }

  // READ request: respond with data
  if (req->request_type == REQ_READ) {
    if (!is_range_valid(req->mem_addr, req->length)) {
#if LOG_INFO
  LOGE("RX", "DEV %02u: NACK - invalid READ range\n", r->addr);
#endif
      return 0;
    }
    resp->request_type = REQ_READ;
    resp->mem_addr = req->mem_addr;
    resp->length = req->length;
    if (resp->length > 0) {
      memcpy(resp->data, &r->memory[req->mem_addr], resp->length);
    }
    frame_compute_checksum(resp);
#if LOG_INFO
  LOGI("RX", "DEV %02u: READ -> ACK to master: addr=0x%04X len=%u crc=0x%04X\n",
        r->addr, resp->mem_addr, resp->length, resp->checksum);
#endif
    return 1;
  }

  // WRITE request
  if (req->request_type == REQ_WRITE) {
      if (!is_range_valid(req->mem_addr, req->length)) {
#if LOG_INFO
  LOGE("RX", "DEV %02u: NACK - invalid WRITE range\n", r->addr);
#endif
        return 0;
      }
      if (!frame_validate_checksum(req)) {
#if LOG_INFO
  LOGE("RX", "DEV %02u: NACK - CRC mismatch on WRITE\n", r->addr);
#endif
        return 0;
      }
      if (req->length > 0) {
        memcpy(&r->memory[req->mem_addr], req->data, req->length);
      }
      resp->length = 0;
      resp->checksum = 0;
#if LOG_INFO
  LOGI("RX", "DEV %02u: WRITE -> ACK to master (len=%u)\n", r->addr, req->length);
#endif
    return 1;
  }

#if LOG_INFO
  LOGE("RX", "DEV %02u: NACK - unknown request type\n", r->addr);
#endif
  return 0;
}

void receiver_dump_memory(const Receiver *r, int max_bytes) {
  if (!r) return;
  if (max_bytes <= 0 || max_bytes > SLAVE_MEM_SIZE) max_bytes = SLAVE_MEM_SIZE;
  printf("\n[device %02u] memory dump (first %d bytes):\n", r->addr, max_bytes);
  for (int i = 0; i < max_bytes; ++i) {
    if (i % 16 == 0) printf("0x%04X: ", i);
    printf("%02X ", r->memory[i]);
    if (i % 16 == 15) printf("\n");
  }
  if (max_bytes % 16 != 0) printf("\n");
}
