#include <stdio.h>
#include <string.h>
#include "receiver.h"
#include "frame.h"
#include "bus.h"
#include "logger.h"
#include "device_store.h"   /* <-- new */

void receiver_init(Receiver* r) {
  if (!r) return;
  memset(r->memory, 0, sizeof(r->memory));
  r->addr       = 0;
  r->have_addr  = 0;
  r->phase      = RX_IDLE;
  r->tmp_byte   = 0;
  r->data_count = 0;
  r->rw         = 0;
  r->bus        = NULL;

  /* Load persisted state (if present) */
  (void)device_store_load(r->memory, SLAVE_MEM_SIZE, NULL);
}

/* Resync */
static inline ack_t rx_resync_on_start(Receiver* r, uint8_t frame) {
  if (l2_is_start(frame)) {
#if LOG_INFO
    LOGI("RX", "START seen -> resync (discard partial state), expecting address\n");
#endif
    r->have_addr  = 0;
    r->data_count = 0;
    r->phase      = RX_WAIT_ADDR;
    return ACK;
  }
  return (ack_t)2;
}

/* Device-side send */
static ack_t device_send_data_with_retry(Receiver* r, uint8_t data) {
  if (!r || !r->bus) return NACK;
  for (int t = 0; t < L3_MAX_RETRIES; ++t) {
    (void)bus_send_from_device(r->bus, l2_make_data(data));
    ack_t ack = bus_send_from_device(r->bus, l2_make_parity_frame(l2_parity_bit8(data)));
    if (ack == ACK) return ACK;
#if LOG_INFO
    LOGE("RX", "Host NACK on data 0x%02X (try %d/%d) -> resend\n", data, t+1, L3_MAX_RETRIES);
#endif
  }
  return NACK;
}

ack_t receiver_on_frame(Receiver* r, uint8_t frame) {
  if (!r) return NACK;

  switch (r->phase) {
  case RX_IDLE: {
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
    if (rs != (ack_t)2) return rs;
    r->tmp_byte = frame;
    r->phase    = RX_WAIT_ADDR_PARITY;
#if LOG_INFO
  LOGV("RX", "ADDR byte=0x%02X (awaiting parity)\n", frame);
#endif
    return ACK;
  }

  case RX_WAIT_ADDR_PARITY: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs;

    uint8_t exp = l2_parity_bit8(r->tmp_byte);
    uint8_t got = l2_get_parity_from_frame(frame);
    if (exp == got) {
      r->addr      = (uint8_t)(r->tmp_byte % SLAVE_MEM_SIZE);
      r->have_addr = 1;
      r->phase     = RX_WAIT_RW;
#if LOG_INFO
      LOGI("RX", "ADDR parity OK (addr=%u) -> WAIT_RW\n", r->addr);
#endif
      return ACK;
    } else {
#if LOG_INFO
      LOGE("RX", "ADDR parity FAIL (byte=0x%02X exp=%u got=%u) -> NACK\n", r->tmp_byte, exp, got);
#endif
      r->phase = RX_WAIT_ADDR;
      return NACK;
    }
  }

  case RX_WAIT_RW: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs;

    r->rw         = l3_get_rw(frame);
    r->data_count = 0;
#if LOG_INFO
    LOGI("RX", "RW=%s\n", (r->rw==L3_RW_WRITE) ? "WRITE" : "READ");
#endif
    if (r->rw == L3_RW_WRITE) {
      r->phase = RX_WAIT_DATA;
      return ACK;
    } else {
      /* READ: push 4 bytes to host with parity and retries */
      int ok = 1;
      for (int i = 0; i < L2_DATA_PER_TX; ++i) {
        uint8_t d = r->memory[r->addr];
        if (device_send_data_with_retry(r, d) != ACK) { ok = 0; break; }
        r->addr = (uint8_t)((r->addr + 1) % SLAVE_MEM_SIZE);
        r->data_count++;
      }
      /* Ready for next TX */
      r->phase      = RX_WAIT_ADDR;
      r->have_addr  = 0;
      r->data_count = 0;
      return ok ? ACK : NACK;
    }
  }

  case RX_WAIT_DATA: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs;

    if (r->rw == L3_RW_READ) {
#if LOG_INFO
      LOGE("RX", "Unexpected DATA from app in READ mode -> NACK\n");
#endif
      return NACK;
    }

    r->tmp_byte = frame;
    r->phase    = RX_WAIT_DATA_PARITY;
#if LOG_INFO
    LOGV("RX", "DATA byte=0x%02X (awaiting parity)\n", frame);
#endif
    return ACK;
  }

  case RX_WAIT_DATA_PARITY: {
    ack_t rs = rx_resync_on_start(r, frame);
    if (rs != (ack_t)2) return rs;

    uint8_t exp = l2_parity_bit8(r->tmp_byte);
    uint8_t got = l2_get_parity_from_frame(frame);
    if (exp == got) {
      r->memory[r->addr] = r->tmp_byte;
      r->addr = (uint8_t)((r->addr + 1) % SLAVE_MEM_SIZE);
      r->data_count++;
#if LOG_INFO
      LOGI("RX", "WRITE OK -> stored (count=%d, next addr=%u)\n", r->data_count, r->addr);
#endif
      if (r->data_count >= L2_DATA_PER_TX) {
#if LOG_INFO
        LOGI("RX", "Transaction complete (WRITE) -> device_app_print() + persist\n");
#endif
        device_app_print(r);
        /* Persist to file after each 4-byte write transaction */
        (void)device_store_save(r->memory, SLAVE_MEM_SIZE, NULL);

        r->data_count = 0;
        r->have_addr  = 0;
        r->phase      = RX_WAIT_ADDR;
      } else {
        r->phase = RX_WAIT_DATA;
      }
      return ACK;
    } else {
#if LOG_INFO
      LOGE("RX", "WRITE parity FAIL (byte=0x%02X exp=%u got=%u) -> NACK\n",
            r->tmp_byte, exp, got);
#endif
      r->phase = RX_WAIT_DATA;
      return NACK;
    }
  }
  } /* switch */

  return NACK;
}
