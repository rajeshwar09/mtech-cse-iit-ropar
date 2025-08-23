#include <stdio.h>    // printf, puts
#include <stdlib.h>   // rand, srand
#include <string.h>
#include <time.h>     // time
#include "bus.h"
#include "logger.h"

// Flip exactly one random bit inside a byte buffer
static void flip_random_bit(uint8_t *bytes, size_t len) {
  if (len == 0) return;                       // nothing to flip
  size_t byte_idx = (size_t)(rand() % (int)len); // pick a random byte
  int bit = rand() % 8;                       // pick a random bit 0..7
  bytes[byte_idx] ^= (uint8_t)(1u << bit);    // toggle that bit
#if LOG_INFO
  LOGV("BUS", "noise: flipped bit %d in byte %zu\n", bit, byte_idx);
#endif
}

// Inject noise in either header, payload, or checksum of a frame
static void apply_noise_to_frame(frame_t *f) {
  int choice = rand() % 3;                    // 0=header, 1=data, 2=checksum
  if (choice == 0) {
    uint8_t header[6];                        // pack header bytes
    header[0] = f->device_addr;
    header[1] = f->packet_type;
    header[2] = f->request_type;
    header[3] = (uint8_t)(f->mem_addr >> 8);
    header[4] = (uint8_t)(f->mem_addr & 0xFF);
    header[5] = f->length;
    flip_random_bit(header, sizeof(header));  // flip one header bit
    // unpack back with clamping
    f->device_addr  = header[0] & MAX_ADDR_VALUE; // keep 6 bits
    f->packet_type  = header[1] ? 1 : 0;          // 0/1
    f->request_type = header[2] ? 1 : 0;          // 0/1
    f->mem_addr     = ((uint16_t)header[3] << 8) | header[4];
    f->length       = header[5];
  } else if (choice == 1 && f->length > 0) {
    flip_random_bit(f->data, f->length);     // flip one payload bit
  } else {
    uint8_t csum[2] = { (uint8_t)(f->checksum >> 8), (uint8_t)(f->checksum & 0xFF) };
    flip_random_bit(csum, 2);                // flip one checksum bit
    f->checksum = ((uint16_t)csum[0] << 8) | csum[1];
  }
}

// Prepare an empty bus with noise probability
void bus_init(Bus *bus, double noise_prob) {
  if (!bus) return;                            // guard
  for (size_t i = 0; i <= MAX_DEVICES_LIMIT; ++i) bus->devices[i] = NULL; // clear table
  // clamp probability to [0,1]
  bus->noise_prob = (noise_prob < 0.0) ? 0.0 : (noise_prob > 1.0 ? 1.0 : noise_prob);
  bus->busy = 0;                               // idle
  srand((unsigned)time(NULL));                 // seed RNG
}

// Register a receiver on its address slot
int bus_attach(Bus *bus, Receiver *r) {
  if (!bus || !r) return 0;                    // guard
  uint8_t a = (r->addr & MAX_ADDR_VALUE);      // 6-bit address
  if (a == MASTER_ADDR || a > MAX_DEVICES_LIMIT) return 0; // reject master/invalid
  bus->devices[a] = r;                         // attach
#if LOG_INFO
  LOGI("BUS", "attached receiver at address %u\n", a);
#endif
  return 1;
}

// Route a request to the destination device and return its response
int bus_send(Bus *bus, const frame_t *req, frame_t *resp_out) {
  static unsigned long next_txid = 1;          // persistent counter
  unsigned long txid = next_txid++;            // unique id for this transfer

  if (!bus || !req || !resp_out) return 0;     // guard
  if (bus->busy) {                              // only one transfer at a time
#if LOG_INFO
    LOGE("BUS", "BUSY -> NACK\n");
#endif
    return 0;
  }
  bus->busy = 1;                                // lock bus

  frame_t local_req = *req;                     // make a modifiable copy
  frame_normalize_headers(&local_req);          // clamp fields

  log_banner_tx_begin(txid, local_req.device_addr); // pretty header
  frame_print_summary("REQ  ▶", &local_req);       // one-line req
  LOGV("BUS", "M->S raw send details: dst=%u type=%u req=%u addr=0x%04X len=%u\n",
       local_req.device_addr, local_req.packet_type, local_req.request_type,
       local_req.mem_addr, local_req.length);

  double r = (double)rand() / RAND_MAX;         // 0..1
  if (r < bus->noise_prob) apply_noise_to_frame(&local_req); // maybe corrupt req

  Receiver *dst = NULL;                         // route to device
  if (local_req.device_addr <= MAX_DEVICES_LIMIT)
    dst = bus->devices[local_req.device_addr];
  if (!dst) {                                   // no device at that address
#if LOG_INFO
    LOGE("BUS", "NACK: no receiver at address %u\n", local_req.device_addr);
#endif
    log_banner_tx_end(txid, 0);
    bus->busy = 0;                              // unlock bus
    return 0;
  }

  frame_t resp_local;                           // device's response lives here
  int ack = receiver_process(dst, &local_req, &resp_local); // call device

  if (ack) {                                    // device ACKed
    r = (double)rand() / RAND_MAX;
    if (r < bus->noise_prob) apply_noise_to_frame(&resp_local); // maybe corrupt resp
    *resp_out = resp_local;                     // return response to caller
    frame_print_summary("RESP ◀", resp_out);   // one-line resp
    LOGV("BUS", "S->M resp details: req=%u addr=0x%04X len=%u crc=0x%04X\n",
         resp_out->request_type, resp_out->mem_addr, resp_out->length, resp_out->checksum);
  } else {
#if LOG_INFO
    LOGE("BUS", "S->M resp: NACK (no ACK produced)\n");
#endif
  }

  log_banner_tx_end(txid, ack);                 // close banner
  bus->busy = 0;                                // unlock bus
  return ack;                                   // 1=ACK, 0=NACK
}
