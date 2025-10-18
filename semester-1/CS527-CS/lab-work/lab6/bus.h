#ifndef BUS_H
#define BUS_H

#include <stdbool.h>
#include "devices.h"

typedef struct Bus {
    StorageDevice dev;
} Bus;

void bus_init(Bus* b);

// Returns true if response produced.
bool bus_transact(Bus* b, const Frame* req, Frame* resp);

#endif // BUS_H