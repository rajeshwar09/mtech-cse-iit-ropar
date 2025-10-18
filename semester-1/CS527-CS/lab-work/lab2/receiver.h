#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

typedef enum {
  RX_IDLE = 0,
  RX_WAIT_ADDR,
  RX_WAIT_ADDR_PARITY,
  RX_WAIT_DATA,
  RX_WAIT_DATA_PARITY
} RxPhase;

typedef struct Receiver {
  /* Lab-2: memory + address */
  uint8_t  memory[SLAVE_MEM_SIZE];
  uint8_t  addr;              /* current write address (0..127) */
  int      have_addr;         /* whether address has been set for current TX */

  /* FSM state */
  RxPhase  phase;
  uint8_t  tmp_byte;          /* holds last received byte (addr or data) awaiting parity */
  int      data_count;        /* 0..L2_DATA_PER_TX */

} Receiver;

/* Initialize receiver state */
void  receiver_init(Receiver* r);

/* Process one incoming byte (start/address/data/parity); return ACK/NACK. */
ack_t receiver_on_frame(Receiver* r, uint8_t frame);

/* Device application: print entire 128B memory in 4 lines × 32 chars. */
void  device_app_print(const Receiver* r);
