#include <stdio.h>
#include "receiver.h"


void device_print_memory(const Receiver *r, int max_bytes) {
    receiver_dump_memory(r, max_bytes);
}
