
#ifndef APP_H
#define APP_H

#include "bus.h"

typedef struct
{
  Bus bus;
  unsigned max_retries;
} App;
void app_init(App *a);
int app_read(unsigned int address, char *message, unsigned int size);
int app_write(unsigned int address, char *message, unsigned int size);

#endif
