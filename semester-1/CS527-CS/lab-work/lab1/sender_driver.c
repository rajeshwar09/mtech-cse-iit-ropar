#include <string.h>
#include "sender_driver.h"
#include "frame.h"
#include "logger.h"
#include "constants.h"

static ack_t send_frame(Bus* bus, uint8_t data4, int start, int end) {
  /* reserved=0 in Lab-1; exposed for future */
  uint8_t f = l1_make_frame_ex(data4, start, 0 /*reserved*/, end);
#if LOG_INFO
  LOGV("TX", "send frame: data4=0x%X start=%d end=%d -> 0x%02X\n",
        data4 & 0x0F, start, end, f);
#endif
  ack_t ack = bus_send(bus, f);
#if LOG_INFO
  LOGI("TX", "frame 0x%02X -> %s\n", f, ack_str(ack));
#endif
  return ack;
}

void app_driver_send_string(Bus* bus, const char* s) {
  if (!bus || !s) return;
  size_t n = strlen(s);
  unsigned long txid = 1; /* Single device, single TX per run */
  log_banner_tx_begin(txid, 1 /*addr*/);

  for (size_t i = 0; i < n; ++i) {
    unsigned char ch = (unsigned char)s[i];
    uint8_t hi = (uint8_t)((ch >> 4) & 0x0F);
    uint8_t lo = (uint8_t)( ch       & 0x0F);

    int is_start_hi = (i == 0) ? 1 : 0;
    int is_end_lo   = (i == n - 1) ? 1 : 0;

    /* Send high nibble then low nibble per byte */
    (void)send_frame(bus, hi, is_start_hi, 0);
    (void)send_frame(bus, lo, 0, is_end_lo);
  }

  log_banner_tx_end(txid, 1 /*ok banner*/);
}
