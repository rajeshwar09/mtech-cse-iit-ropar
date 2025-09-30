#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"

typedef struct Receiver {
  /* For Lab-1: message creation */
  char     msg_buf[MAX_MSG_LEN];
  size_t   msg_len;

  /* Nibble creation state */
  int      have_hi;      /* 0: expecting high nibble; 1: have high nibble */
  uint8_t  hi_nibble;

  /* For Lab-2+: memory (byte addressable) */
  uint8_t  memory[SLAVE_MEM_SIZE];
} Receiver;

/* Initialize receiver state */
void receiver_init(Receiver* r);

/* Process one incoming *frame byte*; return true if accepted (parity OK), false otherwise.
   If the frame marks end-of-transaction, the receiver application will be invoked to print. */
ack_t receiver_on_frame(Receiver* r, uint8_t frame);

/* Device application (prints reconstructed message). */
void device_app_print(const Receiver* r);
