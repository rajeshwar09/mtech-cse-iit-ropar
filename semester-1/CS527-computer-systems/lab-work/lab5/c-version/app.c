
#include "app.h"
#include "frame.h"
#include "constants.h"
#include <string.h>

void app_init(App *a)
{
  a->max_retries = DEFAULT_MAX_RETRIES;
  bus_init(&a->bus);
}

int app_read(unsigned int address, char *message, unsigned int size)
{
  frame_t req = {0}, resp = {0};
  req.packet_type = PKT_DATA;
  req.request_type = REQ_READ;
  req.mem_addr = address;
  req.length = 0;
  for (unsigned attempt = 0; attempt <= DEFAULT_MAX_RETRIES; ++attempt)
  {
    if (bus_send(&((Bus){0}), &req, &resp) && frame_validate_checksum(&resp))
    {
      unsigned n = resp.length;
      if (n > size)
        n = size;
      if (n)
        memcpy(message, resp.data, n);
      return 1;
    }
  }
  if (size)
    message[0] = '\0';
  return 0;
}

int app_write(unsigned int address, char *message, unsigned int size)
{
  frame_t req = {0}, resp = {0};
  req.packet_type = PKT_DATA;
  req.request_type = REQ_WRITE;
  req.mem_addr = address;
  unsigned n = size > 255 ? 255 : size;
  req.length = (uint8_t)n;
  if (n)
    memcpy(req.data, message, n);
  frame_compute_checksum(&req);
  for (unsigned attempt = 0; attempt <= DEFAULT_MAX_RETRIES; ++attempt)
  {
    if (bus_send(&((Bus){0}), &req, &resp) && frame_validate_checksum(&resp))
      return 1;
  }
  return 0;
}