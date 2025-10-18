#include <stdio.h>
#include <string.h>
#include "sender_application.h"
#include "sender_driver.h"
#include "logger.h"

void sender_app_print(Bus* bus, const char* text, uint8_t base_addr) {
  char buf[4096];
  if (!text) text = "";
  int n = snprintf(buf, sizeof(buf), "%s", text);
  if (n < 0) { buf[0] = '\0'; }
  else if ((size_t)n >= sizeof(buf)) buf[sizeof(buf)-1] = '\0';

#if LOG_INFO
  LOGI("APP", "sending (snprintf %%s): \"%s\" at base_addr=%u\n", buf, base_addr);
#endif
  app_driver_send_string_at(bus, buf, base_addr);
}

ack_t sender_app_read4(Bus* bus, uint8_t base_addr, uint8_t out[4]) {
#if LOG_INFO
  LOGI("APP", "request READ4 at base_addr=%u\n", base_addr);
#endif
  return app_driver_read4_at(bus, base_addr, out);
}

ack_t sender_app_readN(Bus* bus, uint8_t base_addr, uint8_t* out, size_t len) {
#if LOG_INFO
  LOGI("APP", "request READ%zu at base_addr=%u\n", len, base_addr);
#endif
  return app_driver_readN_at(bus, base_addr, out, len);
}
