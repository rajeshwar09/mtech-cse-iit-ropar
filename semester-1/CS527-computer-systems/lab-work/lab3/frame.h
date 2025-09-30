#pragma once
#include <stdint.h>

/* ---------------- Lab-3 framing helpers ----------------
  We keep: 8-bit address frame + separate 8-bit parity frame,
          8-bit data frame   + separate 8-bit parity frame.
  Control:
    - START: a fixed token
    - RW:    one control byte with bit0 = (0 read, 1 write)
*/

#define L2_TOKEN_START   ((uint8_t)0xA5)  /* start token, unchanged */

/* popcount & even parity for 8-bit payloads */
static inline uint8_t l2_popcount8(uint8_t x) {
  uint8_t c = 0; for (int i = 0; i < 8; ++i) c += (x >> i) & 1u; return c;
}
static inline uint8_t l2_parity_bit8(uint8_t payload) { return (uint8_t)(l2_popcount8(payload) & 1u); }

/* Builders */
static inline uint8_t l2_make_start(void) { return L2_TOKEN_START; }
static inline uint8_t l2_make_addr(uint8_t addr) { return addr; }
static inline uint8_t l2_make_data(uint8_t data) { return data; }
static inline uint8_t l2_make_parity_frame(uint8_t parity_bit) { return (uint8_t)(parity_bit & 1u); }

/* RW control frame: bit0 carries RW (0=READ,1=WRITE) */
static inline uint8_t l3_make_rw(uint8_t rw_bit) { return (uint8_t)(rw_bit & 1u); }

/* Checkers */
static inline int      l2_is_start(uint8_t b) { return b == L2_TOKEN_START; }
static inline uint8_t  l2_get_parity_from_frame(uint8_t b) { return (uint8_t)(b & 1u); }
static inline uint8_t  l3_get_rw(uint8_t b) { return (uint8_t)(b & 1u); }
