#include <stdio.h>
#include "receiver.h"

/* Device application layer: print the reconstructed message. */
void device_app_print(const Receiver* r) {
  printf("\n[device_app] message: ");
  if (r && r->msg_len > 0) {
    fwrite(r->msg_buf, 1, r->msg_len, stdout);
  }
  printf("\n");
}
