#pragma once
#include <stdint.h>

/* --------- Global constants for Lab 3 --------- */

#define DEFAULT_NOISE_PROB   0.001
#define SLAVE_MEM_SIZE       128
#define L2_DATA_PER_TX       4

/* Lab-3: retries */
#define L3_MAX_RETRIES       5

/* Default file for persistent device memory (can override with LAB_DEVICE_STATE env var) */
#define DEVICE_STATE_FILE    "device_state.bin"

/* ACK/NACK enum */
typedef enum ack_t {
  NACK = 0,
  ACK  = 1
} ack_t;

static inline const char* ack_str(ack_t a) { return a ? "ACK" : "NACK"; }

/* Lab-3: Read/Write flag (bit0) */
enum { L3_RW_READ = 0, L3_RW_WRITE = 1 };

/* Logging toggle kept from Lab-2 */
#ifndef LOG_INFO
#define LOG_INFO 1
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
