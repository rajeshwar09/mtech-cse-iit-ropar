#ifndef DRIVER_H
#define DRIVER_H
#include <stdbool.h>
#include <stdint.h>
#include "bus.h"
typedef struct
{
  Bus *bus;
} DeviceDriver;

void dd_init(DeviceDriver *dd, Bus *bus);
bool dd_read_bytes(DeviceDriver *dd, uint32_t addr, uint32_t len, uint8_t *out);
bool dd_write_bytes(DeviceDriver *dd, uint32_t addr, const uint8_t *in, uint32_t len);
bool dd_read_int(DeviceDriver *dd, uint32_t addr, int32_t *out);
bool dd_write_int(DeviceDriver *dd, uint32_t addr, int32_t v);
bool dd_print(DeviceDriver *dd);
#endif
