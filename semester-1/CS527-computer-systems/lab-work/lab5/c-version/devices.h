
#ifndef DEVICES_H
#define DEVICES_H

#include <stddef.h>
#include <stdint.h>
#include "constants.h"

typedef enum
{
  DEV_KEYBOARD = 0,
  DEV_DISPLAY = 1,
  DEV_ALARM = 2,
  DEV_BUTTON = 3,
  DEV_COUNT = 4
} device_t;

typedef struct Device Device;
typedef int (*dev_read_fn)(Device *, uint32_t addr, uint8_t *out, size_t maxlen, size_t *outlen);
typedef int (*dev_write_fn)(Device *, uint32_t addr, const uint8_t *data, size_t len);

struct Device
{
  device_t kind;
  uint32_t base;
  uint32_t size;
  dev_read_fn on_read;
  dev_write_fn on_write;
};

void devices_init(Device out[DEV_COUNT]);

#endif
