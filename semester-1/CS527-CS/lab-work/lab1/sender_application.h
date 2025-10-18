#pragma once
#include "bus.h"

/* Sender application: formats a plain string via snprintf("%s", text) and sends it. */
void sender_app_print(Bus* bus, const char* text);
