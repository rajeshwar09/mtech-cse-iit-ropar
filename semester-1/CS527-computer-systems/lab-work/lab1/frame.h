#pragma once
#include <stdint.h>

/* Lab-1 frame (MSB→LSB):
   [ Start(1) | Payload(4) | Parity(1) | Reserved(1) | End(1) ]
   - Even parity computed over the 4-bit payload.
*/

enum {
  L1_BIT_START     = 7,
  L1_BIT_END       = 0,
  L1_START_MASK    = (1u << L1_BIT_START),   /* 0x80 */
  L1_END_MASK      = (1u << L1_BIT_END),     /* 0x01 */
  L1_RSVD_MASK     = (1u << 1),              /* reserved bit (bit1)      */
  L1_PARITY_MASK   = (1u << 2),              /* parity bit (bit2)         */
  L1_DATA_SHIFT    = 3,                      /* payload in bits 6..3      */
  L1_DATA_MASK     = (0x0Fu << L1_DATA_SHIFT)
};

static inline uint8_t l1_popcount4(uint8_t x) {
  x &= 0x0F;
  uint8_t c = (x & 1) + ((x >> 1) & 1) + ((x >> 2) & 1) + ((x >> 3) & 1);
  return c;
}

/* Even parity: 1 if payload has odd # of 1s (so total incl. parity becomes even) */
static inline uint8_t l1_compute_parity_bit(uint8_t data4) {
  return (uint8_t)(l1_popcount4(data4) & 1u);
}

/* Extended builder lets you set the reserved bit explicitly (future-proof). */
static inline uint8_t l1_make_frame_ex(uint8_t data4, int start, int reserved, int end) {
  data4 &= 0x0F;
  uint8_t f = (uint8_t)(data4 << L1_DATA_SHIFT);
  if (start)    f |= L1_START_MASK;
  if (end)      f |= L1_END_MASK;
  if (reserved) f |= L1_RSVD_MASK;
  if (l1_compute_parity_bit(data4)) f |= L1_PARITY_MASK;
  return f;
}

/* Default builder with reserved=0 (Lab-1). */
static inline uint8_t l1_make_frame(uint8_t data4, int start, int end) {
  return l1_make_frame_ex(data4, start, 0, end);
}

static inline int      l1_is_start (uint8_t frame) { return (frame & L1_START_MASK) ? 1 : 0; }
static inline int      l1_is_end   (uint8_t frame) { return (frame & L1_END_MASK)   ? 1 : 0; }
static inline int      l1_is_rsvd  (uint8_t frame) { return (frame & L1_RSVD_MASK)  ? 1 : 0; }
static inline uint8_t  l1_get_data4(uint8_t frame) { return (uint8_t)((frame & L1_DATA_MASK) >> L1_DATA_SHIFT); }

static inline int l1_is_valid(uint8_t frame) {
  uint8_t data4 = l1_get_data4(frame);
  uint8_t p     = (frame & L1_PARITY_MASK) ? 1u : 0u;
  return p == l1_compute_parity_bit(data4);
}
