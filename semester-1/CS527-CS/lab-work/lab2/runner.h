#pragma once
#include "constants.h"

/* Central place to hold runtime configuration*/
typedef struct AppConfig {
    const char* data_str;   /* text to send; NULL => prompt interactively */
    double      noise;      /* bit-flip probability on the bus            */
    int         verbose;    /* >0 => LOG_LEVEL_VERBOSE                    */
    int         quiet;      /* >0 => LOG_LEVEL_QUIET                      */
    int         no_color;   /* >0 => disable ANSI colors                  */

    int         have_addr;
    int         addr;       /* 0..127 */
} AppConfig;

void app_config_init(AppConfig* cfg);
int  app_parse_args(int argc, char** argv, AppConfig* cfg);
int  app_interactive_fill(AppConfig* cfg);
ack_t app_run(const AppConfig* cfg);
