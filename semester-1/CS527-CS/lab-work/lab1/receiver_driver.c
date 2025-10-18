#include <stdio.h>
#include <string.h>
#include "receiver.h"
#include "frame.h"
#include "logger.h"

void receiver_init(Receiver* r) {
  if (!r) return;
  memset(r->msg_buf, 0, sizeof(r->msg_buf));
  r->msg_len   = 0;
  r->have_hi   = 0;
  r->hi_nibble = 0;
  memset(r->memory, 0, sizeof(r->memory)); /* Reserved for future */
}

static void rx_reset_message(Receiver* r) {
  r->msg_len = 0;
  r->have_hi = 0;
  r->hi_nibble = 0;
  if (sizeof(r->msg_buf) > 0) r->msg_buf[0] = '\0';
}

ack_t receiver_on_frame(Receiver* r, uint8_t frame) {
  if (!r) return NACK;

#if LOG_INFO
  LOGV("RX", "frame=0x%02X start=%d end=%d rsvd=%d data4=0x%X\n",
        frame, l1_is_start(frame), l1_is_end(frame),
        l1_is_rsvd(frame), l1_get_data4(frame));
#endif

  if (!l1_is_valid(frame)) {
#if LOG_INFO
    LOGE("RX", "parity error on frame 0x%02X -> NACK\n", frame);
#endif
    return NACK;
  }

  if (l1_is_start(frame)) {
#if LOG_INFO
    LOGI("RX", "START detected -> reset assembly\n");
#endif
    rx_reset_message(r);
  }

  uint8_t nib = l1_get_data4(frame);
  if (!r->have_hi) {
    r->hi_nibble = (uint8_t)(nib & 0x0F);
    r->have_hi = 1;
  } else {
    uint8_t byte = (uint8_t)((r->hi_nibble << 4) | (nib & 0x0F));
    if (r->msg_len + 1 < MAX_MSG_LEN) {
      r->msg_buf[r->msg_len++] = (char)byte;
      r->msg_buf[r->msg_len]   = '\0';
    }
    r->have_hi = 0;
  }

  if (l1_is_end(frame)) {
    device_app_print(r);   /* call the device application */
  }

  return ACK;
}
