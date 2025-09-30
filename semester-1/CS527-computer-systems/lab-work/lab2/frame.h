#pragma once
#include <stdint.h>

/* ---------------- Lab-2 frame ----------------
   Updated spec: 8-bit address frame + separate 1-bit parity frame,
   and 8-bit data frame + separate 1-bit parity frame.
   We pack the parity bit into bit0 of a dedicated "parity frame" byte.
   Start is modelled as a dedicated control token.
*/

/* Distinct tokens for control frames (chosen for visibility + low confusion). */
#define L2_TOKEN_START   ((uint8_t)0xA5)  /* "start bit" token */

/* Helpers for parity on 8-bit payloads (even parity) */
static inline uint8_t l2_popcount8(uint8_t x) {
    /* builtin is OK, but let's stay portable */
    uint8_t c = 0;
    for (int i = 0; i < 8; ++i) c += (x >> i) & 1u;
    return c;
}

/* Even parity bit: 1 if popcount(payload) is odd (so total incl. parity becomes even). */
static inline uint8_t l2_parity_bit8(uint8_t payload) {
    return (uint8_t)(l2_popcount8(payload) & 1u);
}

/* Builders */
static inline uint8_t l2_make_start(void) { return L2_TOKEN_START; }
static inline uint8_t l2_make_addr(uint8_t addr) { return addr; }
static inline uint8_t l2_make_data(uint8_t data) { return data; }
static inline uint8_t l2_make_parity_frame(uint8_t parity_bit) { return (uint8_t)(parity_bit & 1u); }

/* Checkers / extractors */
static inline int      l2_is_start(uint8_t b) { return b == L2_TOKEN_START; }
static inline uint8_t  l2_get_parity_from_frame(uint8_t b) { return (uint8_t)(b & 1u); }
