#include "app.h"
#include "constants.h"
#include <stdio.h>

// Elemental helpers to get/set int at MAIN/TEMP index i (0..MAX_N-1)
static inline uint32_t main_addr(unsigned i) { return MAIN_BASE + i * INT_BYTES; }
static inline uint32_t temp_addr(unsigned i) { return TEMP_BASE + i * INT_BYTES; }

// Merge two sorted runs from SRC into DST, entirely via device memory.
// SRC and DST each refer to either MAIN or TEMP region (decided by caller).
// Master uses only a few scalar locals (no arrays), satisfying "no master memory".
static int merge_runs_device(DeviceDriver* dd,
                             int src_is_main,
                             unsigned lo, unsigned mid, unsigned hi,
                             int dst_is_main) // 1 => dst is MAIN, 0 => dst is TEMP
{
    unsigned i = lo, j = mid + 1, k = lo;
    int32_t vi = 0, vj = 0;
    int have_i = 0, have_j = 0;

    while (i <= mid || j <= hi) {
        if (!have_i && i <= mid) {
            uint32_t addr = (src_is_main ? main_addr(i) : temp_addr(i));
            if (!dd_read_int(dd, addr, &vi)) return 0;
            have_i = 1;
        }
        if (!have_j && j <= hi) {
            uint32_t addr = (src_is_main ? main_addr(j) : temp_addr(j));
            if (!dd_read_int(dd, addr, &vj)) return 0;
            have_j = 1;
        }

        int take_left;
        if (i > mid)             take_left = 0;
        else if (j > hi)         take_left = 1;
        else if (vi <= vj)       take_left = 1;
        else                     take_left = 0;

        int32_t outv = take_left ? vi : vj;
        uint32_t waddr = (dst_is_main ? main_addr(k) : temp_addr(k));
        if (!dd_write_int(dd, waddr, outv)) return 0;

        if (take_left) { i++; have_i = 0; }
        else           { j++; have_j = 0; }
        k++;
    }
    return 1;
}

void run_application(DeviceDriver* dd) {
    puts("BEFORE SORT");
    dd_print_device(dd);

    int src_is_main = 1; // start reading from MAIN
    for (unsigned width = 1; width < MAX_N; width <<= 1) {
        int dst_is_main = !src_is_main; // write into the other region
        for (unsigned lo = 0; lo < MAX_N; lo += 2*width) {
            unsigned mid = (lo + width - 1 < MAX_N) ? (lo + width - 1) : (MAX_N - 1);
            unsigned hi  = (lo + 2*width - 1 < MAX_N) ? (lo + 2*width - 1) : (MAX_N - 1);
            if (lo >= MAX_N || lo >= hi) continue;
            if (!merge_runs_device(dd, src_is_main, lo, mid, hi, dst_is_main)) {
                puts("[ERROR] merge failed");
                return;
            }
        }
        // Swap SRC/DST roles
        src_is_main = !src_is_main;
    }

    // Show full device after sort
    puts("=== AFTER SORT ===");
    dd_print_device(dd);
}