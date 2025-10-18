#pragma once
#include <stddef.h>
#include "bus.h"
#include "constants.h"

/* WRITE string starting at base_addr (4 bytes per transaction, retries on NACK). */
void app_driver_send_string_at(Bus* bus, const char* s, uint8_t base_addr);

/* READ len bytes starting at base_addr into 'out' (loops over 4-byte transactions). */
ack_t app_driver_readN_at(Bus* bus, uint8_t base_addr, uint8_t* out, size_t len);

/* Convenience: READ exactly 4 bytes. */
ack_t app_driver_read4_at(Bus* bus, uint8_t base_addr, uint8_t out4[4]);

/* Back-compat convenience (write) */
static inline void app_driver_send_string(Bus* bus, const char* s) {
  app_driver_send_string_at(bus, s, 0);
}
