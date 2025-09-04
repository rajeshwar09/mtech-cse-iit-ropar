
#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include "frame.h"

// Four devices (keyboard, display, alarm, button) are implicitly connected.
// The bus routes by absolute memory address per constants.h.
typedef struct Bus Bus;
struct Bus{ int dummy; };

void bus_init(Bus* b);
int  bus_send(Bus* b, const frame_t* req, frame_t* resp); // 1=ACK, 0=NACK

#endif
