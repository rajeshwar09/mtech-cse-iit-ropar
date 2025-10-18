#include <string.h>
#include "sender_driver.h"

int master_write_string(Bus *bus, uint8_t dst_addr, uint16_t start_addr,
                        const char *str, int max_retries) {
  const uint8_t *bytes = (const uint8_t*)str;
  size_t len = strlen(str);
  return master_write(bus, dst_addr, start_addr, bytes, len, max_retries);
}

int master_read_string(Bus *bus, uint8_t dst_addr, uint16_t start_addr,
                       char *out, size_t out_cap, int max_retries) {
  if (out_cap == 0) return 0;
  if (!master_read(bus, dst_addr, start_addr, (uint8_t*)out, out_cap - 1, max_retries)) {
    return 0;
  }
  out[out_cap - 1] = '\0';
  return 1;
}
