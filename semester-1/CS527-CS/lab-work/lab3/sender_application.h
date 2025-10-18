#pragma once
#include <stdint.h>
#include "bus.h"

/* Write string (snprintf("%s", text)) starting at base_addr */
void sender_app_print(Bus* bus, const char* text, uint8_t base_addr);

/* Read 4 bytes starting at base_addr into out[4]; returns ACK on success */
ack_t sender_app_read4(Bus* bus, uint8_t base_addr, uint8_t out[4]);

/* Read 'len' bytes starting at base_addr into 'out'. Returns ACK on success. */
ack_t sender_app_readN(Bus* bus, uint8_t base_addr, uint8_t* out, size_t len);
