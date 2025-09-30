#include <stdio.h>
#include <ctype.h>
#include "receiver.h"

/* Print the whole 128B memory as 4 lines × 32 chars. Show '.' for zero/non-printables. */
void device_app_print(const Receiver* r) {
  if (!r) return;
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 32; ++col) {
      int idx = row * 32 + col;
      unsigned char ch = r->memory[idx];
      if (ch == 0 || !isprint(ch)) ch = '.';
      putchar(ch);
    }
    putchar('\n');
  }
}
