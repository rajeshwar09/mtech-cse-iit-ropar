#include "logger.h"
#include <stdio.h>

static int g_level = LOG_LEVEL_NORMAL;
static int g_color = 1;
static unsigned long g_txid = 0;

static const char* color_reset = "\x1b[0m";
static const char* color_dim   = "\x1b[2m";
static const char* color_bold  = "\x1b[1m";
static const char* color_green = "\x1b[32m";
static const char* color_red   = "\x1b[31m";
static const char* color_cyan  = "\x1b[36m";

void log_set_level(int lvl) { g_level = lvl; }
int  log_get_level(void)    { return g_level; }

void log_enable_color(int enable) { g_color = enable ? 1 : 0; }
int  log_color_enabled(void)      { return g_color; }

void log_set_txid(unsigned long id) { g_txid = id; }
unsigned long log_get_txid(void)    { return g_txid; }

static void color_begin(const char* c) { if (g_color) fputs(c, stdout); }
static void color_end(void)            { if (g_color) fputs(color_reset, stdout); }

void log_vprintf(int level, const char* tag, const char* fmt, va_list ap) {
    if (level > g_level) return;
    if (tag && *tag) {
        color_begin(color_dim);
        printf("[TX %04lu]", g_txid);
        color_end();
        printf(" ");
        color_begin(color_cyan);
        printf("[%s]", tag);
        color_end();
        printf(" ");
    }
    vprintf(fmt, ap);
}

void log_printf(int level, const char* tag, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_vprintf(level, tag, fmt, ap);
    va_end(ap);
}

void log_draw_rule(void) {
    color_begin(color_dim);
    puts("────────────────────────────────────────────────────────────────────");
    color_end();
}

void log_banner_tx_begin(unsigned long txid, int dst_addr) {
    log_set_txid(txid);
    color_begin(color_bold);
    printf("┌─ TX %04lu: MASTER → DEV %02d\n", txid, dst_addr);
    color_end();
}

void log_banner_tx_end(unsigned long txid, int ok) {
    (void)txid;
    if (ok) {
        color_begin(color_green);
        puts("└─ RESULT: ACK ✓");
    } else {
        color_begin(color_red);
        puts("└─ RESULT: NACK ✗");
    }
    color_end();
}
