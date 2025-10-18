#ifndef DEVICES_H
#define DEVICES_H

#include <stdint.h>
#include <stdbool.h>
#include "frame.h"

// Single storage device with 2048 bytes (ONLY device has memory).
typedef struct StorageDevice {
    uint8_t mem[STORAGE_BYTES];
} StorageDevice;

void device_init(StorageDevice* d);

// Device handlers (return true if a response was produced)
bool device_handle_data (StorageDevice* d, const Frame* req, Frame* resp); // handles both READ & WRITE via rw_flag
bool device_handle_print(StorageDevice* d, const Frame* req, Frame* resp);

static inline bool device_range_ok(uint32_t start, uint32_t len) {
    return (start <= STORAGE_BYTES) && (len <= STORAGE_BYTES) &&
           (start + len <= STORAGE_BYTES);
}

#endif // DEVICES_H