
#include "devices.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int keyboard(Device *d, uint32_t addr, uint8_t *out, size_t maxlen, size_t *outlen)
{
  (void)d;
  (void)addr;
  if (!out || maxlen == 0)
    return 0;
  char buf[KEYBOARD_MEM_BYTES];
  if (!fgets(buf, sizeof(buf), stdin))
    return 0;
  size_t to_copy = strlen(buf);
  if (to_copy > maxlen)
    to_copy = maxlen;
  memcpy(out, buf, to_copy);
  if (outlen)
    *outlen = to_copy;
  return 1;
}

static int display(Device *d, uint32_t a, const uint8_t *data, size_t len)
{
  (void)d;
  (void)a;
  if (len > 0 && data[len - 1] == '\0')
    fputs((const char *)data, stdout);
  else
    fwrite(data, 1, len, stdout);
  fflush(stdout);
  return 1;
}

static int alarm(Device *d, uint32_t a, const uint8_t *data, size_t len)
{
  (void)d;
  (void)a;
  if (!data || len == 0)
    return 0;
  unsigned times = data[0];
  for (unsigned i = 0; i < times; ++i)
    puts("Alarm");
  return 1;
}

static int push(Device *d, uint32_t a, uint8_t *out, size_t maxlen, size_t *outlen)
{
  (void)d;
  (void)a;
  if (!out || maxlen == 0)
    return 0;
  // fputs("Press (0/1): ", stdout);
  // fflush(stdout);
  char buf[16];
  if (!fgets(buf, sizeof(buf), stdin))
    return 0;
  int v = (buf[0] == '1') ? 1 : 0;
  out[0] = (uint8_t)v;
  if (outlen)
    *outlen = 1;
  return 1;
}

void devices_init(Device dev[DEV_COUNT])
{
  dev[DEV_KEYBOARD] = (Device){DEV_KEYBOARD, KEYBOARD_BASE, KEYBOARD_MEM_BYTES, keyboard, 0};
  dev[DEV_DISPLAY] = (Device){DEV_DISPLAY, DISPLAY_BASE, DISPLAY_MEM_BYTES, 0, display};
  dev[DEV_ALARM] = (Device){DEV_ALARM, ALARM_ADDR, ALARM_MEM_BYTES, 0, alarm};
  dev[DEV_BUTTON] = (Device){DEV_BUTTON, BUTTON_ADDR, BUTTON_MEM_BYTES, push, 0};
}
