#include <string.h>
#include "sender_driver.h"
#include "frame.h"
#include "logger.h"
#include "constants.h"

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

static void send_addr_with_parity(Bus* bus, uint8_t addr) {
  (void)send_byte(bus, l2_make_addr(addr));
  (void)send_byte(bus, l2_make_parity_frame(l2_parity_bit8(addr)));
}

static void send_data_with_parity(Bus* bus, uint8_t data) {
  (void)send_byte(bus, l2_make_data(data));
  (void)send_byte(bus, l2_make_parity_frame(l2_parity_bit8(data)));
}

void app_driver_send_string_at(Bus* bus, const char* s, uint8_t base_addr) {
  if (!bus || !s) return;

  size_t n = strlen(s);
  size_t pos = 0;
  unsigned long txid = 1;
  uint8_t addr = base_addr;

  while (pos < n) {
    log_banner_tx_begin(txid++, 1 /*addr: single device*/);

    /* 1) START */
    (void)send_byte(bus, l2_make_start());

    /* 2) ADDR + PARITY */
    send_addr_with_parity(bus, addr);

    /* 3) 4 × DATA + PARITY (pad with '\0' if msg ends) */
    for (int i = 0; i < L2_DATA_PER_TX; ++i) {
      uint8_t byte = (pos < n) ? (uint8_t)s[pos++] : (uint8_t)'\0';
      send_data_with_parity(bus, byte);
      addr = (uint8_t)((addr + 1) % SLAVE_MEM_SIZE);
    }

    log_banner_tx_end(txid-1, 1);
  }
}
