#include <string.h>
#include "sender_driver.h"
#include "crc16.h"
#include "logger.h"

// Runtime-adjustable chunk size (default = MAX_DATA_LEN)
static size_t g_chunk_size = MAX_DATA_LEN;

void sender_set_chunk_size(size_t s) {
  if (s == 0) s = 1;
  if (s > MAX_DATA_LEN) s = MAX_DATA_LEN;
  g_chunk_size = s;
}

static int send_with_retries(Bus *bus, const frame_t *req, frame_t *resp, int max_retries) {
  for (int attempt = 0; attempt <= max_retries; ++attempt) {
    if (bus_send(bus, req, resp)) {
      return 1; // got ACK
    }
    #if LOG_INFO
      LOGE("MASTER", "NACK (attempt %d/%d) -> retrying\n", attempt + 1, max_retries + 1);
    #endif
  }
  return 0; // failed after retries
}

int master_write(Bus *bus, uint8_t dst_addr, uint16_t start_addr,
                 const uint8_t *data, size_t len, int max_retries) {
  if (!bus || !data) return 0;
  size_t offset = 0;
  while (offset < len) {
    size_t chunk = len - offset;
    if (chunk > g_chunk_size) chunk = g_chunk_size;

    frame_t req;
    memset(&req, 0, sizeof(req));
    req.device_addr  = dst_addr & MAX_ADDR_VALUE;
    req.packet_type  = PKT_DATA;
    req.request_type = REQ_WRITE;
    req.mem_addr     = (uint16_t)(start_addr + offset);
    req.length       = (uint8_t)chunk;
    memcpy(req.data, data + offset, chunk);
    frame_compute_checksum(&req);

    frame_t resp;
    int ok = send_with_retries(bus, &req, &resp, max_retries);
    if (!ok) {
    #if LOG_INFO
      LOGE("MASTER", "WRITE failed at offset %zu\n", offset);
    #endif
        return 0;
    }
    offset += chunk;
  }
  return 1;
}

int master_read(Bus *bus, uint8_t dst_addr, uint16_t start_addr,
                uint8_t *out, size_t len, int max_retries) {
  if (!bus || !out) return 0;
  size_t offset = 0;
  while (offset < len) {
    size_t chunk = len - offset;
    if (chunk > g_chunk_size) chunk = g_chunk_size;

    frame_t req;
    memset(&req, 0, sizeof(req));
    req.device_addr  = dst_addr & MAX_ADDR_VALUE;
    req.packet_type  = PKT_DATA;
    req.request_type = REQ_READ;
    req.mem_addr     = (uint16_t)(start_addr + offset);
    req.length       = (uint8_t)chunk;
    req.checksum     = 0; // not used for READ request

    frame_t resp;
    int ok = 0;
    for (int attempt = 0; attempt <= max_retries; ++attempt) {
      if (!bus_send(bus, &req, &resp)) {
        #if LOG_INFO
          LOGE("MASTER", "READ: received NACK (attempt %d/%d)\n", attempt + 1, max_retries + 1);
        #endif
        continue; // try again
      }
      if (!frame_validate_checksum(&resp)) {
        #if LOG_INFO
          LOGE("MASTER", "READ: CRC mismatch on response (attempt %d/%d)\n", attempt + 1, max_retries + 1);
        #endif
        continue; // ask again by resending same read req
      }
      if (resp.mem_addr != (uint16_t)(start_addr + offset) || resp.length != (uint8_t)chunk) {
        #if LOG_INFO
          LOGE("MASTER", "READ: header mismatch (addr or length). Retrying.\n");
        #endif
          continue;
      }
      memcpy(out + offset, resp.data, chunk);
      ok = 1;
      break;
    }
    if (!ok) {
      #if LOG_INFO
        LOGE("MASTER", "READ failed at offset %zu\n", offset);
      #endif
      return 0;
    }
    offset += chunk;
  }
  return 1;
}
