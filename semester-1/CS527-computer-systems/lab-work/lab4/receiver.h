#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdint.h>
#include "constants.h"
#include "frame.h"

typedef struct Receiver {
    uint8_t  addr;                 // 1..63 (0 reserved for master)
    uint8_t  memory[SLAVE_MEM_SIZE];
} Receiver;

// Create / initialize a receiver with the given address.
void receiver_init(Receiver *r, uint8_t address);

// Process a single master DATA frame; produce an ACK or NACK (and data on READ) into 'resp'.
// Return 1 if ACK frame produced, 0 if NACK frame produced.
int receiver_process(Receiver *r, const frame_t *req, frame_t *resp);

// Dump a summary of memory (first N bytes) with device prefix.
void receiver_dump_memory(const Receiver *r, int max_bytes);

#endif // RECEIVER_H
