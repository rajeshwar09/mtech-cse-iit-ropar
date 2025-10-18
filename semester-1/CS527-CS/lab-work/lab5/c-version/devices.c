#include "devices.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int keyboard(Device* d, uint32_t addr, uint8_t* out, size_t maxlen, size_t* outlen) {
  (void)d; (void)addr;
  if (!out || maxlen == 0) return 0;
  /* Lab-5 says “keyboard can call scanf to get a string from the user” */
  /* Read a single token; ensure NUL-termination */
  char buf[KEYBOARD_MEM_BYTES] = {0};
  if (scanf("%127s", buf) != 1) return 0;
  size_t n = strlen(buf) + 1;            /* include NUL */
  if (n > maxlen) n = maxlen;
  memcpy(out, buf, n);
  if (outlen) *outlen = n;
  return 1;
}

static int display(Device* d, uint32_t addr, const uint8_t* data, size_t len) {
  (void)d; (void)addr;
  /* Print as string using standard printf */
  if (len == 0) return 1;
  /* Guarantee NUL-terminated view without copying when possible */
  if (data[len-1] == '\0') printf("%s", (const char*)data);
  else {
    /* Print raw, then a newline */
    fwrite(data, 1, len, stdout);
  }
  fflush(stdout);
  return 1;
}

static int alarm(Device* d, uint32_t addr, const uint8_t* data, size_t len) {
  (void)d; (void)addr;
  if (!data || len == 0) return 0;
  unsigned times = data[0];
  for (unsigned i = 0; i < times; ++i) puts("Alarm");
  return 1;
}

static int push(Device* d, uint32_t addr, uint8_t* out, size_t maxlen, size_t* outlen) {
  (void)d; (void)addr;
  if (!out || maxlen == 0) return 0;
  /* Return 0 or 1 randomly */
  static int seeded = 0; if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
  out[0] = (uint8_t)(rand() & 1);
  if (outlen) *outlen = 1;
  return 1;
}

void devices_init(Device dev[DEV_COUNT]) {
  dev[DEV_KEYBOARD] = (Device){ KEYBOARD_BASE,  KEYBOARD_MEM_BYTES,  keyboard, NULL     };
  dev[DEV_DISPLAY]  = (Device){ DISPLAY_BASE,   DISPLAY_MEM_BYTES,   NULL,     display  };
  dev[DEV_ALARM]    = (Device){ ALARM_ADDR,     ALARM_MEM_BYTES,     NULL,     alarm    };
  dev[DEV_BUTTON]   = (Device){ BUTTON_ADDR,    BUTTON_MEM_BYTES,    push,     NULL     };
}

int devices_route(Device dev[DEV_COUNT], uint32_t a) {
(void)dev;
  /* Keyboard: exact base..base+127 */
  if (a >= KEYBOARD_BASE && a < KEYBOARD_BASE + KEYBOARD_MEM_BYTES) return DEV_KEYBOARD;
  /* Display: base..base+511 */
  if (a >= DISPLAY_BASE  && a < DISPLAY_BASE  + DISPLAY_MEM_BYTES ) return DEV_DISPLAY;
  /* Alarm: exact */
  if (a == ALARM_ADDR) return DEV_ALARM;
  /* Button: exact */
  if (a == BUTTON_ADDR) return DEV_BUTTON;
  return -1;
}
