#include <stdio.h>
#include <string.h>
#include "receiver.h"
#include "frame.h"
#include "logger.h"

void receiver_init(Receiver* r) {
  if (!r) return;
  memset(r->memory, 0, sizeof(r->memory));
  r->addr       = 0;
  r->have_addr  = 0;
  r->phase      = RX_IDLE;
  r->tmp_byte   = 0;
  r->data_count = 0;
}

/* Small helper: resync to new transaction on START seen at any time */
static inline ack_t rx_resync_on_start(Receiver* r, uint8_t frame) {
  if (l2_is_start(frame)) {
#if LOG_INFO
    LOGI("RX", "START seen -> resync (discard partial state), expecting address\n");
#endif
    r->have_addr  = 0;
    r->data_count = 0;
    r->phase      = RX_WAIT_ADDR;
    return ACK; /* acknowledge START */
  }
  return (ack_t)2; /* sentinel = not a START */
}

ack_t receiver_on_frame(Receiver* r, uint8_t frame) {
  if (!r) return NACK;

  switch (r->phase) {
  case RX_IDLE: {
    /* In IDLE we only accept START */
    if (l2_is_start(frame)) {
#if LOG_INFO
        LOGI("RX", "START received -> expecting address\n");
#endif
        r->have_addr  = 0;
        r->data_count = 0;
        r->phase      = RX_WAIT_ADDR;
        return ACK;
    }
#if LOG_INFO
    LOGE("RX", "Unexpected byte 0x%02X in IDLE -> NACK\n", frame);
#endif
    return NACK;
  }

  case RX_WAIT_ADDR: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs; /* handled START */
    r->tmp_byte = frame; /* address candidate */
    r->phase    = RX_WAIT_ADDR_PARITY;
#if LOG_INFO
    LOGV("RX", "ADDR byte=0x%02X (awaiting parity)\n", frame);
#endif
    return ACK; /* acknowledge receipt of address byte */
  }

  case RX_WAIT_ADDR_PARITY: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs; /* handled START: drop partial addr, wait for new one */

    uint8_t expected = l2_parity_bit8(r->tmp_byte);
    uint8_t got      = l2_get_parity_from_frame(frame);
    if (expected == got) {
      r->addr      = (uint8_t)(r->tmp_byte % SLAVE_MEM_SIZE);
      r->have_addr = 1;
      r->phase     = RX_WAIT_DATA;
#if LOG_INFO
      LOGI("RX", "ADDR parity OK (addr=%u)\n", r->addr);
#endif
      return ACK;
    } else {
#if LOG_INFO
      LOGE("RX", "ADDR parity FAIL (addr=0x%02X exp=%u got=%u) -> NACK\n",
            r->tmp_byte, expected, got);
#endif
      r->phase = RX_WAIT_ADDR; /* ask for address again */
      return NACK;
    }
  }

  case RX_WAIT_DATA: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs; /* handled START: new transaction */
    r->tmp_byte = frame; /* data candidate */
    r->phase    = RX_WAIT_DATA_PARITY;
#if LOG_INFO
    LOGV("RX", "DATA byte=0x%02X (awaiting parity)\n", frame);
#endif
    return ACK;
  }

  case RX_WAIT_DATA_PARITY: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs; /* handled START: new transaction */

    uint8_t expected = l2_parity_bit8(r->tmp_byte);
    uint8_t got      = l2_get_parity_from_frame(frame);
    if (expected == got) {
        if (!r->have_addr) {
#if LOG_INFO
      LOGE("RX", "DATA before valid address -> NACK\n");
#endif
          r->phase = RX_WAIT_ADDR;
          return NACK;
        }
        /* store and advance */
        r->memory[r->addr] = r->tmp_byte;
        r->addr = (uint8_t)((r->addr + 1) % SLAVE_MEM_SIZE);
        r->data_count++;
#if LOG_INFO
        LOGI("RX", "DATA parity OK -> stored (count=%d, next addr=%u)\n", r->data_count, r->addr);
#endif
        if (r->data_count >= L2_DATA_PER_TX) {
#if LOG_INFO
          LOGI("RX", "Transaction complete -> device_app_print()\n");
#endif
          device_app_print(r);
          /* Ready for next transaction; next frame may be START or ADDR */
          r->data_count = 0;
          r->have_addr  = 0;
          r->phase      = RX_WAIT_ADDR;
        } else {
          r->phase = RX_WAIT_DATA;
        }
        return ACK;
      } else {
#if LOG_INFO
        LOGE("RX", "DATA parity FAIL (byte=0x%02X exp=%u got=%u) -> NACK\n",
              r->tmp_byte, expected, got);
#endif
        r->phase = RX_WAIT_DATA; /* ask for the data byte again */
        return NACK;
      }
  }
  } /* switch */

  return NACK;
}
