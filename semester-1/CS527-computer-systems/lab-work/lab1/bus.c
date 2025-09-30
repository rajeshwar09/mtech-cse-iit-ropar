#include <stdlib.h>
#include <time.h>
#include "bus.h"
#include "frame.h"
#include "logger.h"
#include "constants.h"

/* Flip exactly one random bit with probability p */
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
  if (noise_prob < 0.0) noise_prob = 0.0;
  if (noise_prob > 1.0) noise_prob = 1.0;
  bus->noise_prob = noise_prob;
  srand((unsigned)time(NULL));
}

ack_t bus_send(Bus* bus, uint8_t frame) {
  if (!bus || !bus->dev) return NACK;

  uint8_t noisy = maybe_flip(frame, bus->noise_prob);
#if LOG_INFO
  LOGV("BUS", "TX 0x%02X -> RX 0x%02X (start=%d end=%d rsvd=%d data4=0x%X)\n",
        frame, noisy, l1_is_start(noisy), l1_is_end(noisy),
        l1_is_rsvd(noisy), l1_get_data4(noisy));
#endif
  ack_t ack = receiver_on_frame(bus->dev, noisy);
#if LOG_INFO
  LOGI("BUS", "device returned %s\n", ack_str(ack));
#endif
  return ack;
}
