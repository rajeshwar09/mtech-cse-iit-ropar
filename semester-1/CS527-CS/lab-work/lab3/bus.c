#include <stdlib.h>
#include <time.h>
#include "bus.h"
#include "frame.h"
#include "logger.h"
#include "constants.h"

static uint8_t maybe_flip(uint8_t b, double p) {
  double r = (double)rand() / (double)RAND_MAX;
  if (r < p) {
    int bit = rand() % 8;
    uint8_t out = (uint8_t)(b ^ (uint8_t)(1u << bit));
#if LOG_INFO
    LOGV("BUS", "noise: flipped bit %d (0x%02X -> 0x%02X)\n", bit, b, out);
#endif
    return out;
  }
  return b;
}

void bus_init(Bus* bus, Receiver* dev, double noise_prob) {
  if (!bus) return;
  bus->dev = dev;
  if (bus->dev) bus->dev->bus = bus; /* allow device to push frames back */
  if (noise_prob < 0.0) noise_prob = 0.0;
  if (noise_prob > 1.0) noise_prob = 1.0;
  bus->noise_prob = noise_prob;
  bus->host_cb = NULL;
  bus->host_ctx = NULL;
  srand((unsigned)time(NULL));
}

void bus_attach_host(Bus* bus, bus_host_on_frame_cb cb, void* ctx) {
  if (!bus) return;
  bus->host_cb  = cb;
  bus->host_ctx = ctx;
}

/* App -> Device */
ack_t bus_send(Bus* bus, uint8_t frame) {
  if (!bus || !bus->dev) return NACK;
  uint8_t noisy = maybe_flip(frame, bus->noise_prob);
#if LOG_INFO
  LOGV("BUS", "TX byte 0x%02X -> RX 0x%02X\n", frame, noisy);
#endif
  ack_t ack = receiver_on_frame(bus->dev, noisy);
#if LOG_INFO
  LOGI("BUS", "device returned %s\n", ack_str(ack));
#endif
  return ack;
}

/* Device -> App (during READ) */
ack_t bus_send_from_device(Bus* bus, uint8_t frame) {
  if (!bus || !bus->host_cb) return NACK;
  uint8_t noisy = maybe_flip(frame, bus->noise_prob);
#if LOG_INFO
  LOGV("BUS", "DEV->APP byte 0x%02X -> host sees 0x%02X\n", frame, noisy);
#endif
  ack_t ack = bus->host_cb(bus->host_ctx, noisy);
#if LOG_INFO
  LOGI("BUS", "host returned %s\n", ack_str(ack));
#endif
  return ack;
}
