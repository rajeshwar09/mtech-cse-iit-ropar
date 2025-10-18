#pragma once
#include <stddef.h>
#include "constants.h"

typedef struct AppConfig {
  const char* data_str; /* for write; NULL => prompt */
  double      noise;
  int         verbose;
  int         quiet;
  int         no_color;

  int         have_addr;
  int         addr;       /* 0..127 */

  int         read_mode;  /* 1 => READ instead of WRITE */
  size_t      read_len;   /* bytes to read (default 4) */
} AppConfig;

void app_config_init(AppConfig* cfg);
int  app_parse_args(int argc, char** argv, AppConfig* cfg);
int  app_interactive_fill(AppConfig* cfg);

/* Separate run functions so main can print READ output (as required) */
ack_t app_run_write(const AppConfig* cfg);

ack_t app_run_read(const AppConfig* cfg, char* out_buf, size_t len);