#ifndef SENDER_DRIVER_H
#define SENDER_DRIVER_H

#include <stddef.h>
#include <stdint.h>
#include "bus.h"

// Write 'len' bytes from 'data' into 'dst_addr' starting at 'start_addr'.
// Retries on NACK up to 'max_retries' times PER frame. Returns 1 on success.
int master_write(Bus *bus, uint8_t dst_addr, uint16_t start_addr,
                 const uint8_t *data, size_t len, int max_retries);

// Read 'len' bytes from 'dst_addr' starting at 'start_addr' into 'out' buffer.
// Retries read requests and validates CRC on responses. Returns 1 on success.
int master_read(Bus *bus, uint8_t dst_addr, uint16_t start_addr,
                uint8_t *out, size_t len, int max_retries);

// NEW: set runtime chunk size (clamped to 1..MAX_DATA_LEN (255))
void sender_set_chunk_size(size_t s);

#endif // SENDER_DRIVER_H
