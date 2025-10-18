#ifndef CONSTANTS_H
#define CONSTANTS_H

// Addresses
#define MASTER_ADDR     0x00
#define MAX_ADDR_VALUE  0x3F

// Memory & framing
#define SLAVE_MEM_SIZE  4096
#define MAX_DATA_LEN    255
#define CRC16_POLY      0x1021
#define CRC16_INIT      0xFFFF

// Bus
#define DEFAULT_NOISE_PROB  0.01
#define MAX_DEVICES_LIMIT   63

// Retries
#define DEFAULT_MAX_RETRIES 3

#define LOG_INFO 1

#endif // CONSTANTS_H
