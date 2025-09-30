#include "devices.h"
#include "crc.h"
#include "constants.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void device_init(StorageDevice *d)
{
  srand((unsigned)time(NULL));
  for (uint32_t i = 0; i < STORAGE_BYTES; i++)
    d->mem[i] = (uint8_t)(rand() & 0xFF);
}

bool device_data(StorageDevice *d, const Frame *req, Frame *resp)
{
  if (!d || !req || !resp)
    return false;
  if (req->device_id != DEV_ID_STORAGE)
    return false;
  if (!dev_range_ok(req->start_addr, req->length))
    return false;
  if (req->rw_flag == RW_READ)
  {
    memset(resp, 0, sizeof(*resp));
    resp->device_id = DEV_ID_STORAGE;
    resp->packet_type = PKT_TYPE_ACK;
    resp->rw_flag = RW_READ;
    resp->start_addr = req->start_addr;
    resp->length = req->length;
    memcpy(resp->payload, &d->mem[req->start_addr], req->length);
    resp->crc = crc_payload(resp->payload, resp->length);
    return true;
  }
  else
  {
    uint16_t want = crc_payload(req->payload, req->length);
    memset(resp, 0, sizeof(*resp));
    resp->device_id = DEV_ID_STORAGE;
    resp->packet_type = PKT_TYPE_ACK;
    resp->rw_flag = RW_WRITE;
    resp->start_addr = req->start_addr;
    resp->length = 1;
    if (want != req->crc)
    {
      resp->payload[0] = 0;
      resp->crc = crc_payload(resp->payload, resp->length);
      return true;
    }
    memcpy(&d->mem[req->start_addr], req->payload, req->length);
    resp->payload[0] = 1;
    resp->crc = crc_payload(resp->payload, resp->length);
    return true;
  }
}

bool device_print(StorageDevice *d, const Frame *req, Frame *resp)
{
  (void)req;
  if (!d || !resp)
    return false;
  for (uint32_t i = 0; i < MAX_N * INT_BYTES; i += INT_BYTES)
  {
    uint32_t addr = MAIN_BASE + i;
    int32_t v = 0;
    memcpy(&v, &d->mem[addr], INT_BYTES);
    printf("%d\n", v);
  }
  fflush(stdout);
  memset(resp, 0, sizeof(*resp));
  resp->device_id = DEV_ID_STORAGE;
  resp->packet_type = PKT_TYPE_ACK;
  resp->rw_flag = RW_READ;
  resp->start_addr = MAIN_BASE;
  resp->length = 1;
  resp->payload[0] = 1;
  resp->crc = crc_payload(resp->payload, resp->length);
  return true;
}
