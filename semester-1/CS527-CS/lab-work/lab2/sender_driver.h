#pragma once
#include "bus.h"
#include "constants.h"

/* Lab-2 application driver:
  - Splits the message into blocks of 4 bytes.
  - For each block: START, ADDR, PARITY(addr), then 4× {DATA, PARITY(data)}.
*/
void app_driver_send_string_at(Bus* bus, const char* s, uint8_t base_addr);

/* Back-compat convenience wrapper (defaults base address = 0). */
static inline void app_driver_send_string(Bus* bus, const char* s) {
  app_driver_send_string_at(bus, s, 0);
}
