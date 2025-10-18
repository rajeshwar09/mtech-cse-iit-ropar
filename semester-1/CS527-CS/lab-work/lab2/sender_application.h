#pragma once
#include <stdint.h>
#include "bus.h"

/* Sender application: formats a plain string via snprintf("%s", text) and sends it
   starting at 'base_addr' in the receiver's 128-byte memory. */
void sender_app_print(Bus* bus, const char* text, uint8_t base_addr);
