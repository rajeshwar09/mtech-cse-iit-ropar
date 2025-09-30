#include "bus.h"
#include <string.h>

void bus_init(Bus* b) {
  if (!b) return;
  devices_init(b->devices);
}

/* Protocol same as Lab-4:
  - READ request: length=0, no data; bus returns ACK with payload and checksum.
  - WRITE request: length>0, data present; bus validates req checksum, performs write, returns ACK with len=0. */
int bus_send(Bus* b, const frame_t* req, frame_t* resp) {
  if (!b || !req || !resp) return 0;

  /* Default NACK response template */
  memset(resp, 0, sizeof(*resp));
  resp->packet_type  = PKT_ACK;
  resp->request_type = req->request_type;   /* keep for clarity */
  resp->mem_addr     = req->mem_addr;
  resp->length       = 0;                   /* default no payload in ACK */

  int idx = devices_route(b->devices, req->mem_addr);
  if (idx < 0) return 0; /* invalid address => NACK */

  Device* d = &b->devices[idx];

  if (req->request_type == REQ_READ) {
    if (!d->on_read) return 0;           /* read not supported => NACK */
    size_t outlen = 0;
    if (!d->on_read(d, req->mem_addr, resp->data, FRAME_DATA_MAX, &outlen)) return 0;
    if (outlen > FRAME_DATA_MAX) outlen = FRAME_DATA_MAX;
    resp->length = (uint8_t)outlen;      /* return payload */
    frame_compute_checksum(resp);
    return 1;
  } else { /* WRITE */
    if (!d->on_write) return 0;          /* write not supported => NACK */
    /* Validate request's checksum on data bytes */
    if (!frame_validate_checksum(req)) return 0;
    if (!d->on_write(d, req->mem_addr, req->data, req->length)) return 0;
    resp->length = 0;
    frame_compute_checksum(resp);        /* CRC over empty data (fine) */
    return 1;
  }
}
