#include "device_store.h"
#include "constants.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char* pick_path(const char* path) {
  const char* env = getenv("LAB_DEVICE_STATE");
  if (path && *path) return path;
  if (env && *env)   return env;
  return DEVICE_STATE_FILE; /* from constants.h */
}

int device_store_load(uint8_t* mem, size_t n, const char* path) {
  if (!mem || n == 0) return -1;

  const char* p = pick_path(path);
  FILE* f = fopen(p, "rb");
  if (!f) {
#if LOG_INFO
      LOGV("STORE", "no state file; starting fresh (%s)\n", p);
#endif
      return 0; /* not an error: file missing is fine */
  }

  size_t rd = fread(mem, 1, n, f);
  fclose(f);

  if (rd < n) {
      /* If file shorter than n, zero-fill remainder */
      memset(mem + rd, 0, n - rd);
  }
#if LOG_INFO
  LOGI("STORE", "loaded %zu bytes from %s\n", n, p);
#endif
  return 0;
}

int device_store_save(const uint8_t* mem, size_t n, const char* path) {
  if (!mem || n == 0) return -1;

  const char* p = pick_path(path);
  FILE* f = fopen(p, "wb");
  if (!f) {
#if LOG_INFO
      LOGE("STORE", "failed to open %s for write\n", p);
#endif
      return -2;
  }
  size_t wr = fwrite(mem, 1, n, f);
  fclose(f);
  if (wr != n) {
#if LOG_INFO
      LOGE("STORE", "short write to %s (%zu/%zu)\n", p, wr, n);
#endif
      return -3;
  }
#if LOG_INFO
  LOGI("STORE", "saved %zu bytes to %s\n", n, p);
#endif
  return 0;
}
