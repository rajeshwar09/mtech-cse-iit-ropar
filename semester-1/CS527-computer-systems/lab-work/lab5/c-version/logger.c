
#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

static int g_level = LOG_LEVEL_NORMAL;
static int g_color = 1;

static const char *color_reset = "\x1b[0m";
static const char *color_cyan = "\x1b[36m";
static const char *color_dim = "\x1b[2m";

void log_set_level(int level) { g_level = level; }
int log_get_level(void) { return g_level; }
void log_enable_color(int enable) { g_color = enable ? 1 : 0; }
int log_color_enabled(void) { return g_color; }

void log_vprintf(int level, const char *tag, const char *fmt, va_list ap)
{
  if (level > g_level)
    return;
  if (g_color)
    fputs(color_cyan, stdout);
  if (tag)
    printf("[%s] ", tag);
  if (g_color)
    fputs(color_reset, stdout);
  vprintf(fmt, ap);
}

void log_printf(int level, const char *tag, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  log_vprintf(level, tag, fmt, ap);
  va_end(ap);
}

void log_draw_rule(void)
{
  if (g_color)
    fputs(color_dim, stdout);
  puts("------------------------------------------------------------");
  if (g_color)
    fputs(color_reset, stdout);
}

void log_banner_tx_begin(unsigned long txid, int dst_addr)
{
  log_draw_rule();
  LOGI("TX", "begin id=%lu dst=0x%08X\n", txid, (unsigned)dst_addr);
}

void log_banner_tx_end(unsigned long txid, int ok)
{
  LOGI("TX", "end   id=%lu status=%s\n", txid, ok ? "OK" : "NACK");
  log_draw_rule();
}
