#include "app.h"
#include "constants.h"
#include <stdio.h>

static inline uint32_t A(unsigned i) { return MAIN_BASE + i * INT_BYTES; }
static inline uint32_t T(unsigned i) { return TEMP_BASE + i * INT_BYTES; }

static int mergesort(DeviceDriver *dd, int src_is_main, unsigned lo, unsigned mid, unsigned hi, int dst_is_main)
{
  unsigned i = lo, j = mid + 1, k = lo;
  int32_t vi = 0, vj = 0;
  int hi_v = 0, hj_v = 0;
  while (i <= mid || j <= hi)
  {
    if (!hi_v && i <= mid)
    {
      if (!dd_read_int(dd, src_is_main ? A(i) : T(i), &vi))
        return 0;
      hi_v = 1;
    }
    if (!hj_v && j <= hi)
    {
      if (!dd_read_int(dd, src_is_main ? A(j) : T(j), &vj))
        return 0;
      hj_v = 1;
    }
    int left;
    if (i > mid)
      left = 0;
    else if (j > hi)
      left = 1;
    else if (vi <= vj)
      left = 1;
    else
      left = 0;
    int32_t outv = left ? vi : vj;
    if (!dd_write_int(dd, dst_is_main ? A(k) : T(k), outv))
      return 0;
    if (left)
    {
      i++;
      hi_v = 0;
    }
    else
    {
      j++;
      hj_v = 0;
    }
    k++;
  }
  return 1;
}

void run_app(DeviceDriver *dd)
{
  puts("BEFORE SORT");
  dd_print(dd);
  int src_is_main = 1;
  for (unsigned w = 1; w < MAX_N; w <<= 1)
  {
    int dst_is_main = !src_is_main;
    for (unsigned lo = 0; lo < MAX_N; lo += 2 * w)
    {
      unsigned mid = (lo + w - 1 < MAX_N) ? lo + w - 1 : MAX_N - 1;
      unsigned hi = (lo + 2 * w - 1 < MAX_N) ? lo + 2 * w - 1 : MAX_N - 1;

      int r = mergesort(dd, src_is_main, lo, mid, hi, dst_is_main);
    }
    src_is_main = !src_is_main;
  }
  if (!src_is_main)
  {
    for (unsigned i = 0; i < MAX_N; i++)
    {
      int32_t v;
      if (!dd_read_int(dd, T(i), &v))
        return;
      if (!dd_write_int(dd, A(i), v))
        return;
    }
  }
  puts("\nAFTER SORT");
  dd_print(dd);
}
