#include "app.h"
#include "frame.h"
#include <string.h>

/* Keep a single Bus instance for the app */
static Bus g_bus;

void app_init(void) {
  bus_init(&g_bus);
}

/* READ: build a frame with length=0, no data; copy response payload into message. */
void read(unsigned int address, char* message, unsigned int size) {
  if (!message || size == 0) return;

  frame_t req = {0}, resp = {0};
  req.packet_type  = PKT_DATA;
  req.request_type = REQ_READ;
  req.mem_addr     = (uint32_t)address;
  req.length       = 0;
  req.checksum     = 0;  /* CRC over empty payload is optional; receiver ignores on READ */

  if (bus_send(&g_bus, &req, &resp) && resp.packet_type == PKT_ACK) {
    /* Copy up to 'size-1' and NUL-terminate for stringy devices */
    unsigned n = resp.length;
    if (n >= size) n = size - 1;
    if (n > 0) memcpy(message, resp.data, n);
    message[n] = '\0';
  } else {
    /* NACK or error: return empty string */
    message[0] = '\0';
  }
}

/* WRITE: send up to 255 bytes; compute checksum on data bytes as per spec. */
void write(unsigned int address, char* message, unsigned int size) {
  if (!message || size == 0) return;

  frame_t req = {0}, resp = {0};
  req.packet_type  = PKT_DATA;
  req.request_type = REQ_WRITE;
  req.mem_addr     = (uint32_t)address;

  unsigned n = size;
  if (n > FRAME_DATA_MAX) n = FRAME_DATA_MAX;
  memcpy(req.data, message, n);
  req.length = (uint8_t)n;
  frame_compute_checksum(&req);

  (void)bus_send(&g_bus, &req, &resp); /* ignore NACKs per Lab-5 (no retry requirement stated) */
}
