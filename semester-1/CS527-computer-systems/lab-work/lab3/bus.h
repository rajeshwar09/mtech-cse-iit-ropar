#pragma once
#include <stdint.h>
#include "constants.h"
#include "receiver.h"

/* Host callback so device can send frames back to the application during READ. */
typedef ack_t (*bus_host_on_frame_cb)(void* ctx, uint8_t frame);

typedef struct Bus {
  double    noise_prob;
  Receiver* dev;

  /* Lab-3 additions for device->host path */
  bus_host_on_frame_cb host_cb;
  void*                host_ctx;
} Bus;

void  bus_init(Bus* bus, Receiver* dev, double noise_prob);
void  bus_attach_host(Bus* bus, bus_host_on_frame_cb cb, void* ctx);

/* App -> Device */
ack_t bus_send(Bus* bus, uint8_t frame);

/* Device -> App (used only during READ transactions) */
ack_t bus_send_from_device(Bus* bus, uint8_t frame);
