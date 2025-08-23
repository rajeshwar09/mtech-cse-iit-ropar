#ifndef BUS_H
#define BUS_H

#include <stddef.h>
#include "frame.h"
#include "receiver.h"
#include "constants.h"

typedef struct {
  Receiver *devices[MAX_DEVICES_LIMIT + 1]; // index by address (0..63), 0 = master reserved (NULL)
  double noise_prob;
  int busy;
} Bus;

void bus_init(Bus *bus, double noise_prob);
int  bus_attach(Bus *bus, Receiver *r);
int  bus_send(Bus *bus, const frame_t *req, frame_t *resp_out);

#endif // BUS_H
