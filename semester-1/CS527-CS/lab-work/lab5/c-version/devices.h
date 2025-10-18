#pragma once
#include <stdint.h>
#include <stddef.h>
#include "constants.h"

typedef struct Device Device;

/* Read callback returns 1 on success and fills out/outlen; Write returns 1 on success. */
typedef int (*dev_read_cb)(Device*, uint32_t addr, uint8_t* out, size_t maxlen, size_t* outlen);
typedef int (*dev_write_cb)(Device*, uint32_t addr, const uint8_t* data, size_t len);

struct Device {
  uint32_t    base;
  uint32_t    size;      /* bytes (1 for alarm/button; 128 keyboard; 512 display) */
  dev_read_cb on_read;   /* NULL if write-only */
  dev_write_cb on_write; /* NULL if read-only */
};

enum { DEV_COUNT = 4, DEV_KEYBOARD = 0, DEV_DISPLAY = 1, DEV_ALARM = 2, DEV_BUTTON = 3 };

void devices_init(Device dev[DEV_COUNT]);
/* Finds device index by absolute address, returns -1 if none. */
int  devices_route(Device dev[DEV_COUNT], uint32_t addr);
