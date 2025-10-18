#pragma once
#include "bus.h"
#include "constants.h"

/* Application driver: sends a string by encoding each byte into two frames (MSN then LSN).
   Marks the very first frame with START=1 and the very last frame with END=1. */
void app_driver_send_string(Bus* bus, const char* s);
