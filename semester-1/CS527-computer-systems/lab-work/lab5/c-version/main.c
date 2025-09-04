
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "app.h"
#include "constants.h"

static int convert_int(const char *s, int *number)
{
  char *end = NULL;
  long v = strtol(s, &end, 10);
  if (end == s)
    return 0;
  *number = (int)v;
  return 1;
}

int main(void)
{
  App app;
  app_init(&app);

  char msg[] = "Welcome to CS527: Computer Systems \nLet's have fun\n";
  app_write(DISPLAY_BASE, msg, sizeof(msg));

  while (1)
  {
    char msg[] = "Write a random number\n";
    app_write(DISPLAY_BASE, msg, sizeof(msg));

    char buff[KEYBOARD_MEM_BYTES] = {0};
    app_read(KEYBOARD_BASE, buff, sizeof(buff));
    int number = 0;
    convert_int(buff, &number);

    if (number < 24)
    {
      unsigned char btn = 0;
      app_read(BUTTON_ADDR, (char *)&btn, 1u);
      if (btn == 1)
      {
        unsigned char nbyte = (unsigned char)(number & 0xFF);
        app_write(ALARM_ADDR, (char *)&nbyte, 1u);
        break;
      }
      else
        continue;
    }
    else
      continue;
  }
  return 0;
}
