#pragma once
#include "frame.h"
#include "devices.h"

typedef struct Bus {
  Device devices[DEV_COUNT];
} Bus;

void bus_init(Bus* b);
int  bus_send(Bus* b, const frame_t* req, frame_t* resp); /* 1=ACK, 0=NACK */
