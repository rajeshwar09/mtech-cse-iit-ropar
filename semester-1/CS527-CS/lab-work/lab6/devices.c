#include "devices.h"
#include "crc16.h"
#include "constants.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void device_init(StorageDevice* d) {
  if (!d) return;
  srand((unsigned)time(NULL));
  for (uint32_t i = 0; i < STORAGE_BYTES; ++i) {
    d->mem[i] = (uint8_t)(rand() & 0xFF);
  }
}

bool device_handle_data(StorageDevice* d, const Frame* req, Frame* resp) {
  if (!d || !req || !resp) return false;
  if (req->device_id != DEV_ID_STORAGE) return false;
  if (!device_range_ok(req->start_addr, req->length)) return false;

  if (req->rw_flag == RW_READ) {
    memset(resp, 0, sizeof(*resp));
    resp->device_id   = DEV_ID_STORAGE;
    resp->packet_type = PKT_TYPE_ACKNAK;
    resp->rw_flag     = RW_READ;
    resp->start_addr  = req->start_addr;
    resp->length      = req->length;
    memcpy(resp->payload, &d->mem[req->start_addr], req->length);
    resp->crc         = crc16_payload(resp->payload, resp->length);
    return true;
  } else {
    uint16_t want = crc16_payload(req->payload, req->length);
    memset(resp, 0, sizeof(*resp));
    resp->device_id   = DEV_ID_STORAGE;
    resp->packet_type = PKT_TYPE_ACKNAK;
    resp->rw_flag     = RW_WRITE;
    resp->start_addr  = req->start_addr;
    resp->length      = 1u;

    if (want != req->crc) {
        resp->payload[0]  = 0u;
        resp->crc         = crc16_payload(resp->payload, resp->length);
        return true;
    }
    memcpy(&d->mem[req->start_addr], req->payload, req->length);
    resp->payload[0]  = 1u;
    resp->crc         = crc16_payload(resp->payload, resp->length);
    return true;
  }
}

static void device_print_all(const StorageDevice* d) {
  for (uint32_t i = 0; i < STORAGE_BYTES; i += INT_BYTES) {
      int32_t v = 0;
      memcpy(&v, &d->mem[i], INT_BYTES);
      printf("%d\n", v);
  }
  fflush(stdout);
}

bool device_handle_print(StorageDevice* d, const Frame* req, Frame* resp) {
  (void)req;
  if (!d || !resp) return false;
  device_print_all(d);

  memset(resp, 0, sizeof(*resp));
  resp->device_id   = DEV_ID_STORAGE;
  resp->packet_type = PKT_TYPE_ACKNAK;
  resp->rw_flag     = RW_READ;
  resp->start_addr  = 0u;
  resp->length      = 1u;
  resp->payload[0]  = 1u; // ACK
  resp->crc         = crc16_payload(resp->payload, resp->length);
  return true;
}