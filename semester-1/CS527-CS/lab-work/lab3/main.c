#include <stdio.h>
#include <stdlib.h>
#include "runner.h"

int main(int argc, char** argv) {
  AppConfig cfg;
  app_config_init(&cfg);

  int pr = app_parse_args(argc, argv, &cfg);
  if (pr != 0) return (pr == 1 ? 0 : 1);

  if (app_interactive_fill(&cfg) != 0) return 1;

  if (cfg.read_mode) {
    char* s = (char*)malloc(cfg.read_len + 1);
    if (!s) return 2;
    if (app_run_read(&cfg, s, cfg.read_len) == ACK) {
      printf("[main] READ%zu result: \"%s\"\n", cfg.read_len, s);
      free(s);
      return 0;
    } else {
      fprintf(stderr, "[main] READ failed\n");
      free(s);
      return 2;
    }
  } else {
    return app_run_write(&cfg) == ACK ? 0 : 2;
  }
}
