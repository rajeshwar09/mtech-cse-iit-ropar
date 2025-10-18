#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

// Single storage device with a 5-bit id (0..31)
#define DEV_ID_STORAGE      ((uint8_t)0x03)

#define PKT_TYPE_DATA       ((uint8_t)0u)
#define PKT_TYPE_ACKNAK     ((uint8_t)1u)
#define PKT_TYPE_PRINT      ((uint8_t)2u)

#define RW_READ             ((uint8_t)0u)
#define RW_WRITE            ((uint8_t)1u)

// Device memory: 2048 bytes total (ONLY device has memory)
// This 2KB holds BOTH: main array + temp array (no extra memory elsewhere)
#define STORAGE_BYTES       (2048u)
#define INT_BYTES           (4u)
#define STORAGE_INTS        (STORAGE_BYTES/INT_BYTES) // 512

// MAX_N integers will be sorted. Constraint: main+temp = 2*MAX_N*4 <= 2048  => MAX_N <= 256
#ifndef MAX_N
#define MAX_N (256u)
#endif

#if (MAX_N == 0) || (MAX_N > 256)
#error "MAX_N must be in 1..256 since 2*MAX_N*4 must fit into 2048B."
#endif

// Decide start addresses of MAIN and TEMP regions (bytes) at the start (condition #5)
#ifndef MAIN_BASE
#define MAIN_BASE (0u)                 // default: main starts at 0
#endif

#ifndef TEMP_BASE
#define TEMP_BASE ( (MAX_N) * (INT_BYTES) ) // default: temp starts right after main
#endif

// Sanity: ensure both regions fit into device memory with no overlap beyond 2*MAX_N*4
#if (MAIN_BASE + (MAX_N)*INT_BYTES > STORAGE_BYTES)
#error "MAIN region exceeds device memory."
#endif
#if (TEMP_BASE + (MAX_N)*INT_BYTES > STORAGE_BYTES)
#error "TEMP region exceeds device memory."
#endif

// CRC16-CCITT parameters (payload-only CRC, as Lab-4)
#define CRC16_POLY          ((uint16_t)0x1021u)
#define CRC16_INIT          ((uint16_t)0xFFFFu)

// Retry policy (same spirit as Lab-4 driver)
#define MAX_RETRIES         (3u)

#endif // CONSTANTS_H