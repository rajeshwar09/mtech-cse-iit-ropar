#pragma once
#include "bus.h"

/* Initialize the application’s connection to the bus. */
void app_init(void);

/* Lab-5: Application has only two functions to interact with drivers */
void read (unsigned int address, char* message, unsigned int size); /* fills message (NUL-terminated if space permits) */
void write(unsigned int address, char* message, unsigned int size); /* sends size bytes (truncated to 255 if larger) */
