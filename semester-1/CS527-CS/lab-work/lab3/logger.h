#pragma once
#include <stdarg.h>

/* Logging levels */
#define LOG_LEVEL_VERBOSE 0
#define LOG_LEVEL_NORMAL  1
#define LOG_LEVEL_QUIET   2

void log_set_level(int lvl);
int  log_get_level(void);

void log_enable_color(int enable);
int  log_color_enabled(void);

void log_set_txid(unsigned long id);
unsigned long log_get_txid(void);

void log_vprintf(int level, const char* tag, const char* fmt, va_list ap);
void log_printf (int level, const char* tag, const char* fmt, ...);

/* Pretty banners and rules */
void log_draw_rule(void);
void log_banner_tx_begin(unsigned long txid, int dst_addr);
void log_banner_tx_end  (unsigned long txid, int ok);

/* Convenience macros matching your style */
#define LOGV(tag, fmt, ...) log_printf(LOG_LEVEL_VERBOSE, tag, fmt, ##__VA_ARGS__)
#define LOGI(tag, fmt, ...) log_printf(LOG_LEVEL_NORMAL,  tag, fmt, ##__VA_ARGS__)
#define LOGE(tag, fmt, ...) log_printf(LOG_LEVEL_NORMAL,  tag, fmt, ##__VA_ARGS__)
