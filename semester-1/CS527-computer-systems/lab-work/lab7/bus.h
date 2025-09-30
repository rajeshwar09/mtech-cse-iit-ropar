#ifndef BUS_H
#define BUS_H
#include <stdbool.h>
#include "devices.h"
typedef struct
{
  StorageDevice dev;
} Bus;
void bus_init(Bus *b);
bool bus_transact(Bus *b, const Frame *req, Frame *resp);
#endif
