#include "bus.h"
#include "constants.h"
#include "logger.h"
#include "devices.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static Device g_devs[DEV_COUNT];

static double rand01(void) {
  return (double)rand() / ((double)RAND_MAX + 1.0);
}

static void flip_random_bit(uint8_t *bytes, size_t len) {
  if (!bytes || len == 0) return;
  size_t i = (size_t)(rand() % (int)len);
  int bit = rand() % 8;
  bytes[i] ^= (uint8_t)(1u << bit);
}

static Device *route(uint32_t addr)
{
  if (addr >= KEYBOARD_BASE && addr < KEYBOARD_BASE + KEYBOARD_MEM_BYTES)
    return &g_devs[DEV_KEYBOARD];
  if (addr >= DISPLAY_BASE && addr <= DISPLAY_END)
    return &g_devs[DEV_DISPLAY];
  if (addr == ALARM_ADDR)
    return &g_devs[DEV_ALARM];
  if (addr == BUTTON_ADDR)
    return &g_devs[DEV_BUTTON];
  return NULL;
}

void bus_init(Bus *b)
{
  (void)b;
  devices_init(g_devs);
}

int bus_send(Bus *b, const frame_t *req, frame_t *resp)
{
  (void)b;
  if (!req || !resp)
    return 0;
  memset(resp, 0, sizeof(*resp));
  resp->packet_type  = PKT_ACK;
  resp->request_type = req->request_type;
  resp->mem_addr     = req->mem_addr;

  Device *d = route(req->mem_addr);
  if (!d)
  {
    LOGI("BUS", "Invalid address 0x%08X\n", (unsigned)req->mem_addr);
    return 0;
  }

  int ok = 0;
  if (req->request_type == REQ_READ)
  {
    size_t outlen = 0;
    ok = d->on_read(d, req->mem_addr, resp->data, sizeof(resp->data), &outlen);
    resp->length = (uint8_t)(outlen <= 255 ? outlen : 255);
    frame_compute_checksum(resp);

    if (ok && rand01() < BUS_ERROR_PROB) {
      if (resp->length > 0) {
        flip_random_bit(resp->data, resp->length);
      } else {
        resp->checksum ^= (uint16_t)(1u << (rand() % 16));
      }
    }
  }
  else
  {
    ok = d->on_write(d, req->mem_addr, req->data, req->length);
    resp->length = 0;
    frame_compute_checksum(resp);

    if (ok && rand01() < BUS_ERROR_PROB) {
      resp->checksum ^= (uint16_t)(1u << (rand() % 16));
    }
  }
  return ok ? 1 : 0;
}