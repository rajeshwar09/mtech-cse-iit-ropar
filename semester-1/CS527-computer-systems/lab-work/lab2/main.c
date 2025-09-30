#include "runner.h"

int main(int argc, char** argv) {
  AppConfig cfg;
  app_config_init(&cfg);

  int pr = app_parse_args(argc, argv, &cfg);
  if (pr != 0) return (pr == 1 ? 0 : 1);  /* help printed => 0; error => 1 */

  if (app_interactive_fill(&cfg) != 0) return 1;

  return app_run(&cfg) == ACK ? 0 : 2;
}
