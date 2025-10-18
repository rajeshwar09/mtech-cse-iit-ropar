#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <stdint.h>

#define DEV_ID_STORAGE ((uint8_t)0x03)
#define PKT_TYPE_DATA ((uint8_t)0)
#define PKT_TYPE_ACK ((uint8_t)1)
#define PKT_TYPE_PRINT ((uint8_t)2)
#define RW_READ ((uint8_t)0)
#define RW_WRITE ((uint8_t)1)

#define STORAGE_BYTES (2048u)
#define INT_BYTES (4u)
#define STORAGE_INTS (STORAGE_BYTES / INT_BYTES)

#ifndef MAX_N
#define MAX_N (256u)
#endif

#ifndef MAIN_BASE
#define MAIN_BASE (0u)
#endif
#ifndef TEMP_BASE
#define TEMP_BASE ((MAX_N) * INT_BYTES)
#endif

#define CRC16_POLY ((uint16_t)0x1021)
#define CRC16_INIT ((uint16_t)0xFFFF)
#define MAX_RETRIES (3u)
#endif
