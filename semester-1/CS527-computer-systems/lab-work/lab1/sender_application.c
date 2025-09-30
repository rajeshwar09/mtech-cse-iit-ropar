#include <stdio.h>
#include <string.h>
#include "sender_application.h"
#include "sender_driver.h"
#include "logger.h"

void sender_app_print(Bus* bus, const char* text) {
  char buf[4096];
  if (!text) text = "";

  int n = snprintf(buf, sizeof(buf), "%s", text);
  if (n < 0) {
#if LOG_INFO
    LOGE("APP", "snprintf failed; sending empty string\n");
#endif
    buf[0] = '\0';
  } else if ((size_t)n >= sizeof(buf)) {
    buf[sizeof(buf) - 1] = '\0';  /* truncated, but NUL-terminated */
  }

#if LOG_INFO
  LOGI("APP", "sending (snprintf %%s): \"%s\"\n", buf);
#endif
  app_driver_send_string(bus, buf);
}
