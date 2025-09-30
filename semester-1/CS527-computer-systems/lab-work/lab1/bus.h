#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
#include "receiver.h"

typedef struct Bus {
  double    noise_prob;
  Receiver* dev;       /* single device for Lab-1 */
} Bus;

void  bus_init(Bus* bus, Receiver* dev, double noise_prob);
/* Send one *frame byte* over the bus; returns ACK if device accepted (parity OK). */
ack_t bus_send(Bus* bus, uint8_t frame);
