#include <string.h>
#include "sender_driver.h"
#include "frame.h"
#include "logger.h"
#include "constants.h"

/* ---------------- Common helpers ---------------- */

static ack_t send_byte(Bus* bus, uint8_t byte) {
#if LOG_INFO
  LOGV("TX", "send byte: 0x%02X\n", byte);
#endif
  ack_t ack = bus_send(bus, byte);
#if LOG_INFO
  LOGI("TX", "byte 0x%02X -> %s\n", byte, ack_str(ack));
#endif
  return ack;
}

static ack_t send_with_retry(Bus* bus, uint8_t byte) {
  for (int t = 0; t < L3_MAX_RETRIES; ++t) {
      if (send_byte(bus, byte) == ACK) return ACK;
#if LOG_INFO
      LOGE("TX", "NACK -> resend 0x%02X (try %d/%d)\n", byte, t+1, L3_MAX_RETRIES);
#endif
  }
  return NACK;
}

static ack_t send_addr_with_parity_retry(Bus* bus, uint8_t addr) {
  for (int t = 0; t < L3_MAX_RETRIES; ++t) {
    if (send_byte(bus, l2_make_addr(addr)) != ACK) { continue; }
    if (send_byte(bus, l2_make_parity_frame(l2_parity_bit8(addr))) == ACK) return ACK;
#if LOG_INFO
    LOGE("TX", "ADDR parity NACK -> resend address (try %d/%d)\n", t+1, L3_MAX_RETRIES);
#endif
  }
  return NACK;
}

static ack_t send_data_with_parity_retry(Bus* bus, uint8_t data) {
  for (int t = 0; t < L3_MAX_RETRIES; ++t) {
    if (send_byte(bus, l2_make_data(data)) != ACK) { continue; }
    if (send_byte(bus, l2_make_parity_frame(l2_parity_bit8(data))) == ACK) return ACK;
#if LOG_INFO
    LOGE("TX", "DATA parity NACK -> resend data 0x%02X (try %d/%d)\n", data, t+1, L3_MAX_RETRIES);
#endif
  }
  return NACK;
}

/* ---------------- WRITE path (like Lab-2, with R/W + retries) ---------------- */

void app_driver_send_string_at(Bus* bus, const char* s, uint8_t base_addr) {
  if (!bus || !s) return;

  size_t n = strlen(s);
  size_t pos = 0;
  unsigned long txid = 1;
  uint8_t addr = base_addr;

  while (pos < n) {
    log_banner_tx_begin(txid++, 1 /*device id*/);

    /* START (retry too, in case BUS returns NACK for some reason) */
    if (send_with_retry(bus, l2_make_start()) != ACK) {
      LOGE("TX", "START failed after retries. Abort.\n");
      log_banner_tx_end(txid-1, 0);
      return;
    }

    /* ADDR + PARITY */
    if (send_addr_with_parity_retry(bus, addr) != ACK) {
      LOGE("TX", "ADDRESS failed after retries. Abort.\n");
      log_banner_tx_end(txid-1, 0);
      return;
    }

    /* RW=WRITE */
    if (send_with_retry(bus, l3_make_rw(L3_RW_WRITE)) != ACK) {
      LOGE("TX", "RW(WRITE) failed after retries. Abort.\n");
      log_banner_tx_end(txid-1, 0);
      return;
    }

    /* 4 × DATA + PARITY (pad with '\0') */
    for (int i = 0; i < L2_DATA_PER_TX; ++i) {
      uint8_t byte = (pos < n) ? (uint8_t)s[pos++] : (uint8_t)'\0';
      if (send_data_with_parity_retry(bus, byte) != ACK) {
        LOGE("TX", "DATA failed after retries. Abort.\n");
        log_banner_tx_end(txid-1, 0);
        return;
      }
      addr = (uint8_t)((addr + 1) % SLAVE_MEM_SIZE);
    }

    log_banner_tx_end(txid-1, 1);
  }
}

/* ---------------- READ path: device -> app with ACK/NACK from host ---------------- */

/* Host-side tiny FSM to receive 4×(DATA+PARITY) and validate parity. */
typedef struct {
  uint8_t buf[4];
  int     idx;        /* 0..3 */
  int     expect_par; /* 0=expect DATA, 1=expect PARITY */
  uint8_t last_data;
  int     done;       /* set to 1 when 4 bytes collected */
} ReadSession;

static ack_t host_on_frame(void* ctx, uint8_t frame) {
  ReadSession* rs = (ReadSession*)ctx;
  if (!rs) return NACK;

  if (!rs->expect_par) {
    /* Expecting DATA */
    rs->last_data = frame;
    rs->expect_par = 1;
#if LOG_INFO
    LOGV("APP", "HOST got DATA 0x%02X (awaiting parity)\n", frame);
#endif
    return ACK; /* acknowledge receipt of data byte */
  } else {
    /* Expecting PARITY for last_data */
    uint8_t exp = l2_parity_bit8(rs->last_data);
    uint8_t got = l2_get_parity_from_frame(frame);
    if (exp == got) {
      if (rs->idx < 4) rs->buf[rs->idx++] = rs->last_data;
      rs->expect_par = 0;
#if LOG_INFO
      LOGI("APP", "HOST parity OK (idx=%d)\n", rs->idx);
#endif
      if (rs->idx >= 4) rs->done = 1;
      return ACK;
    } else {
#if LOG_INFO
      LOGE("APP", "HOST parity FAIL (data=0x%02X exp=%u got=%u)\n", rs->last_data, exp, got);
#endif
      /* NACK: ask device to resend same DATA+PARITY */
      rs->expect_par = 0; /* expect data again */
      return NACK;
    }
  }
}

ack_t app_driver_read4_at(Bus* bus, uint8_t base_addr, uint8_t out4[4]) {
  if (!bus || !out4) return NACK;

  /* Attach host callback */
  ReadSession rs; memset(&rs, 0, sizeof(rs));
  bus_attach_host(bus, host_on_frame, &rs);

  unsigned long txid = 1;

  log_banner_tx_begin(txid++, 1);

  /* START */
  if (send_with_retry(bus, l2_make_start()) != ACK) {
    LOGE("TX", "START failed\n");
    log_banner_tx_end(txid-1, 0);
    return NACK;
  }

  /* ADDR + PARITY */
  if (send_addr_with_parity_retry(bus, base_addr) != ACK) {
    LOGE("TX", "ADDR failed\n");
    log_banner_tx_end(txid-1, 0);
    return NACK;
  }

  /* RW=READ */
  if (send_with_retry(bus, l3_make_rw(L3_RW_READ)) != ACK) {
    LOGE("TX", "RW(READ) failed\n");
    log_banner_tx_end(txid-1, 0);
    return NACK;
  }

  /* From here, device pushes 4× (DATA+PARITY) to host via bus_send_from_device().
      Our host_on_frame() will return ACK/NACK until 4 valid bytes arrive. */
  /* Busy-wait here: in this simple model, device will synchronously push data
      as a consequence of the RW state; no extra app->device triggers are needed. */

  /* Spin until host received all 4 bytes or we decide to timeout (not needed here) */
  while (!rs.done) {
      /* In this synchronous simulation, device will have already completed;
          loop yields just in case of future timing changes. */
  }

  memcpy(out4, rs.buf, 4);
  log_banner_tx_end(txid-1, 1);
  return ACK;
}

ack_t app_driver_readN_at(Bus* bus, uint8_t base_addr, uint8_t* out, size_t len) {
  if (!bus || !out || len == 0) return NACK;

  size_t remaining = len;
  uint8_t addr = base_addr;
  size_t offset = 0;

  while (remaining > 0) {
    uint8_t chunk[4] = {0,0,0,0};
    if (app_driver_read4_at(bus, addr, chunk) != ACK) {
      return NACK;
    }
    size_t take = remaining < 4 ? remaining : 4;
    memcpy(out + offset, chunk, take);

    offset     += take;
    remaining  -= take;
    addr        = (uint8_t)((addr + 4) % SLAVE_MEM_SIZE);  /* advance by a TX worth of data */
  }
  return ACK;
}