#include "driver.h"
#include "crc.h"
#include <string.h>

void dd_init(DeviceDriver *dd, Bus *bus)
{
  dd->bus = bus;
}

static bool transfer(DeviceDriver *dd, const Frame *req, Frame *resp)
{
  return bus_transact(dd->bus, req, resp);
}

bool dd_read_bytes(DeviceDriver *dd, uint32_t addr, uint32_t len, uint8_t *out)
{
  Frame req = {0}, resp = {0};
  req.device_id = DEV_ID_STORAGE;
  req.packet_type = PKT_TYPE_DATA;
  req.rw_flag = RW_READ;
  req.start_addr = addr;
  req.length = len;
  for (int a = 0; a <= MAX_RETRIES; a++)
  {
    memset(&resp, 0, sizeof(resp));
    if (!transfer(dd, &req, &resp))
      continue;
    if (resp.packet_type != PKT_TYPE_ACK)
      continue;
    if (resp.length != len)
      continue;
    if (crc_payload(resp.payload, resp.length) != resp.crc)
      continue;
    memcpy(out, resp.payload, len);
    return true;
  }
  return false;
}

bool dd_write_bytes(DeviceDriver *dd, uint32_t addr, const uint8_t *in, uint32_t len)
{
  Frame req = {0}, resp = {0};
  req.device_id = DEV_ID_STORAGE;
  req.packet_type = PKT_TYPE_DATA;
  req.rw_flag = RW_WRITE;
  req.start_addr = addr;
  req.length = len;
  if (len)
    memcpy(req.payload, in, len);
  req.crc = crc_payload(req.payload, req.length);
  for (int a = 0; a <= MAX_RETRIES; a++)
  {
    memset(&resp, 0, sizeof(resp));
    if (!transfer(dd, &req, &resp))
      continue;
    if (resp.packet_type != PKT_TYPE_ACK)
      continue;
    if (resp.length != 1)
      continue;
    if (crc_payload(resp.payload, resp.length) != resp.crc)
      continue;
    if (resp.payload[0] == 1)
      return true;
  }
  return false;
}

bool dd_read_int(DeviceDriver *dd, uint32_t addr, int32_t *out)
{
  return dd_read_bytes(dd, addr, INT_BYTES, (uint8_t *)out);
}

bool dd_write_int(DeviceDriver *dd, uint32_t addr, int32_t v)
{
  return dd_write_bytes(dd, addr, (const uint8_t *)&v, INT_BYTES);
}

bool dd_print(DeviceDriver *dd)
{
  Frame req = {0}, resp = {0};
  req.device_id = DEV_ID_STORAGE;
  req.packet_type = PKT_TYPE_PRINT;
  if (!transfer(dd, &req, &resp))
    return false;
  if (resp.packet_type != PKT_TYPE_ACK)
    return false;
  if (resp.length != 1)
    return false;
  if (crc_payload(resp.payload, resp.length) != resp.crc)
    return false;
  return resp.payload[0] == 1;
}
