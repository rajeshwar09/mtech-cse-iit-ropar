#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdint.h>
#include "constants.h"
#include "frame.h"

typedef struct Receiver {
    uint8_t  addr;                 // 1..63 (0 reserved for master)
    uint8_t  memory[SLAVE_MEM_SIZE];
} Receiver;


void receiver_init(Receiver *r, uint8_t address);

int receiver_process(Receiver *r, const frame_t *req, frame_t *resp);

void receiver_dump_memory(const Receiver *r, int max_bytes);

#endif 
