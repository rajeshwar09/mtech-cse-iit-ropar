#pragma once
#include <stddef.h>
#include <stdint.h>
#include "constants.h"

/* Forward declare Bus to avoid circular include */
struct Bus;

typedef enum {
  RX_IDLE = 0,
  RX_WAIT_ADDR,
  RX_WAIT_ADDR_PARITY,
  RX_WAIT_RW,
  RX_WAIT_DATA,
  RX_WAIT_DATA_PARITY
} RxPhase;

typedef struct Receiver {
  uint8_t  memory[SLAVE_MEM_SIZE];
  uint8_t  addr;
  int      have_addr;

  RxPhase  phase;
  uint8_t  tmp_byte;
  int      data_count;

  /* Lab-3: RW mode for current transaction (0=READ,1=WRITE) */
  uint8_t  rw;

  /* Lab-3: bus back-reference so device can send during READ */
  struct Bus* bus;
} Receiver;

void  receiver_init(Receiver* r);
ack_t receiver_on_frame(Receiver* r, uint8_t frame);
void  device_app_print(const Receiver* r);
