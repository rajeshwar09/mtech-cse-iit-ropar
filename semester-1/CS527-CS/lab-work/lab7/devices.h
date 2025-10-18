#ifndef DEVICES_H
#define DEVICES_H
#include <stdint.h>
#include <stdbool.h>
#include "frame.h"
typedef struct
{
  uint8_t mem[STORAGE_BYTES];
} StorageDevice;

void device_init(StorageDevice *d);
bool device_data(StorageDevice *d, const Frame *req, Frame *resp);
bool device_print(StorageDevice *d, const Frame *req, Frame *resp);
static inline bool dev_range_ok(uint32_t a, uint32_t l) { return a <= STORAGE_BYTES && l <= STORAGE_BYTES && a + l <= STORAGE_BYTES; }
#endif
