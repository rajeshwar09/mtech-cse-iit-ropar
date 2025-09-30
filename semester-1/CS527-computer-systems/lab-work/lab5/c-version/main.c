#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#include "constants.h"

int main(void) {
  app_init();

  /* a. Welcome string to display */
  const char* s = "Welcome to CS527: Computer Systems \nLet's have fun\n";
  /* Include terminating NUL so Display prints as string cleanly */
  char buf[128];
  snprintf(buf, sizeof(buf), "%s", s);
  write(DISPLAY_BASE, buf, (unsigned)(strlen(buf) + 1));

  /* Loop per pseudo-code */
  for (;;) {
    /* b. Ask user to write a random number */
    {
      const char* s = "Write a random number";
      char buf[64];
      snprintf(buf, sizeof(buf), "%s", s);
      write(DISPLAY_BASE, buf, (unsigned)(strlen(buf) + 1));
    }

    /* c. Expect a number by sending a read request to the keyboard */
    char in[KEYBOARD_MEM_BYTES] = {0};
    read(KEYBOARD_BASE, in, sizeof(in));

    /* Convert input string to integer */
    int value = atoi(in);

    /* d/e. Branch by value */
    if (value < 24) {
      /* Read push button */
      char btnbuf[2] = {0};
      read(BUTTON_ADDR, btnbuf, sizeof(btnbuf));
      unsigned char btn = (unsigned char)btnbuf[0];

      if (btn == 1) {
        /* i. If push button sends 1, write to alarm with number received */
        char out[1];
        out[0] = (char)(value & 0xFF);
        write(ALARM_ADDR, out, 1);
        break; /* Done */
      } else {
        /* ii. Else go to 'b' (loop continues) */
        continue;
      }
    } else {
      /* e. Else go to 'b' (loop continues) */
      continue;
    }
  }

  return 0;
}
