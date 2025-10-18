#ifndef DRIVER_H
#define DRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include "bus.h"

typedef struct DeviceDriver {
    Bus* bus;
} DeviceDriver;

void dd_init(DeviceDriver* dd, Bus* bus);

// Byte I/O (elemental) — no bulk buffers beyond immediate payload
bool dd_read_bytes (DeviceDriver* dd, uint32_t addr, uint32_t len, uint8_t* out);
bool dd_write_bytes(DeviceDriver* dd, uint32_t addr, const uint8_t* in, uint32_t len);

// Single int helpers (keep master memory to scalars only)
bool dd_read_int (DeviceDriver* dd, uint32_t byte_addr, int32_t* out);
bool dd_write_int(DeviceDriver* dd, uint32_t byte_addr, int32_t value);

// Ask device to print its entire memory
bool dd_print_device(DeviceDriver* dd);

#endif // DRIVER_H