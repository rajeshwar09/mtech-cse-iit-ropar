#include "frame.h"
#include "crc16.h"

void frame_compute_checksum(frame_t* f) {
  if (!f) return;
  f->checksum = crc16_ccitt_false(f->data, f->length);
}

int frame_validate_checksum(const frame_t* f) {
  if (!f) return 0;
  return (crc16_ccitt_false(f->data, f->length) == f->checksum) ? 1 : 0;
}
