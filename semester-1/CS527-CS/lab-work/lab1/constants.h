#pragma once
#include <stdint.h>

/* --------- Global constants for Lab 1 --------- */

#define DEFAULT_NOISE_PROB   0.001   /* Probability to flip one random bit in a frame */
#define MAX_MSG_LEN          4096    /* Max reconstructed message size at receiver    */
#define SLAVE_MEM_SIZE       128     /* Reserved for Lab2+ (byte-addressable memory) */

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
