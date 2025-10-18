#pragma once
#include <stdint.h>

/* --------- Global constants for Lab 2 --------- */

#define DEFAULT_NOISE_PROB   0.001   /* Probability to flip one random bit in a frame */
#define MAX_MSG_LEN          4096    /* not used in lab2 but kept for continuity      */
#define SLAVE_MEM_SIZE       128     /* Byte-addressable memory at receiver           */
#define L2_DATA_PER_TX       4       /* Exactly 4 data bytes per transaction          */

/* ACK/NACK enum for handshake-like returns across layers */
typedef enum ack_t {
  NACK = 0,
  ACK  = 1
} ack_t;

static inline const char* ack_str(ack_t a) { return a ? "ACK" : "NACK"; }

/* Enable/disable compile-time guarded logs present in source via #if LOG_INFO */
#ifndef LOG_INFO
#define LOG_INFO 1
#endif

/* Utility macros */
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
