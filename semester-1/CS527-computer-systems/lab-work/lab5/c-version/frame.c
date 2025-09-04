
#include "frame.h"
#include "crc16.h"

void frame_compute_checksum(frame_t *f)
{
  f->checksum = crc16_ccitt(f->data, f->length);
}

int frame_validate_checksum(const frame_t *f)
{
  return f->checksum == crc16_ccitt(f->data, f->length);
}
