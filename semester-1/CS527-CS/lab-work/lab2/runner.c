#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runner.h"
#include "bus.h"
#include "receiver.h"
#include "sender_application.h"
#include "logger.h"

static void app_usage(const char* prog) {
  printf("Usage:\n");
  printf("  %s [--data \"STRING\"] [--addr N] [--noise P] [--verbose|--quiet] [--no-color] [--help]\n", prog);
  printf("\nNotes:\n");
  printf("  - If --data is omitted, the sender app will prompt for a string and format via snprintf.\n");
  printf("  - --addr N selects the starting write address (0..127). Default 0 if not given.\n");
  printf("  - --noise P sets bit-flip probability (default %.3g).\n", DEFAULT_NOISE_PROB);
}

void app_config_init(AppConfig* cfg) {
  if (!cfg) return;
  cfg->data_str = NULL;
  cfg->noise    = DEFAULT_NOISE_PROB;
  cfg->verbose  = 0;
  cfg->quiet    = 0;
  cfg->no_color = 0;
  cfg->have_addr = 0;
  cfg->addr      = 0;
  log_set_level(LOG_LEVEL_NORMAL);
}

int app_parse_args(int argc, char** argv, AppConfig* cfg) {
  if (!cfg) return 1;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--data") && i + 1 < argc) {
      cfg->data_str = argv[++i];
    } else if (!strcmp(argv[i], "--addr") && i + 1 < argc) {
      cfg->addr = atoi(argv[++i]);
      cfg->have_addr = 1;
      if (cfg->addr < 0) cfg->addr = 0;
      if (cfg->addr > 127) cfg->addr = 127;
    } else if (!strcmp(argv[i], "--noise") && i + 1 < argc) {
      cfg->noise = atof(argv[++i]);
      if (cfg->noise < 0.0) cfg->noise = 0.0;
      if (cfg->noise > 1.0) cfg->noise = 1.0;
    } else if (!strcmp(argv[i], "--verbose")) {
      cfg->verbose = 1; cfg->quiet = 0; log_set_level(LOG_LEVEL_VERBOSE);
    } else if (!strcmp(argv[i], "--quiet")) {
      cfg->quiet = 1; cfg->verbose = 0; log_set_level(LOG_LEVEL_QUIET);
    } else if (!strcmp(argv[i], "--no-color")) {
      cfg->no_color = 1; log_enable_color(0);
    } else if (!strcmp(argv[i], "--help")) {
      app_usage(argv[0]);
      return 1;
    } else {
      printf("Unknown argument: %s\n", argv[i]);
      app_usage(argv[0]);
      return 2;
    }
  }
  return 0;
}

int app_interactive_fill(AppConfig* cfg) {
  if (!cfg) return 1;
  if (!cfg->data_str) {
    static char buf[8192];
    printf("Enter string to send: ");
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
    if (!fgets(buf, sizeof(buf), stdin)) {
      fprintf(stderr, "Failed to read input.\n");
      return 1;
    }
    size_t blen = strlen(buf);
    if (blen > 0 && buf[blen - 1] == '\n') buf[--blen] = '\0';
    cfg->data_str = buf;
  }
  if (!cfg->have_addr) {
    printf("Enter base address (0..127): ");
    int addr_in = 0;
    if (scanf("%d", &addr_in) != 1) return 1;
    if (addr_in < 0) addr_in = 0;
    if (addr_in > 127) addr_in = 127;
    cfg->addr = addr_in;
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
  }
  return 0;
}

ack_t app_run(const AppConfig* cfg) {
  if (!cfg) return NACK;

  if (cfg->no_color) log_enable_color(0);
  if (cfg->verbose)  log_set_level(LOG_LEVEL_VERBOSE);
  if (cfg->quiet)    log_set_level(LOG_LEVEL_QUIET);

  Receiver dev;
  receiver_init(&dev);

  Bus bus;
  bus_init(&bus, &dev, cfg->noise);

  log_draw_rule();
  sender_app_print(&bus, cfg->data_str ? cfg->data_str : "", (uint8_t)cfg->addr);
  log_draw_rule();

  return ACK;
}
