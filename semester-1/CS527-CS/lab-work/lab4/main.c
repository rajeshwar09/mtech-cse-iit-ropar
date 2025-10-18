// main.c (with --state DIR persistence)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "constants.h"
#include "bus.h"
#include "receiver.h"
#include "sender_driver.h"
#include "logger.h"

static void usage(const char *prog)
{
  printf("Usage:\n");
  printf("  %s --devices N --dest D --write --addr A --data \"STRING\" [--retries R] [--noise P] [--dump] [--state DIR] [--chunk N]\n", prog);
  printf("  %s --devices N --dest D --read  --addr A --len  L       [--retries R] [--noise P] [--dump] [--state DIR] [--chunk N]\n", prog);
  printf("\nNotes:\n");
  printf("  - Addresses are 6-bit: 0 reserved for master, 1..63 for slaves.\n");
  printf("  - A is 16-bit memory address (0..%d).\n", SLAVE_MEM_SIZE - 1);
  printf("  - L is number of bytes to read (1..%d).\n", MAX_DATA_LEN);
  printf("  - P is noise probability in [0.0, 1.0].\n");
  printf("  - With --dump, device memory prefix is printed after operation.\n");
  printf("  - Use --verbose or --quiet to control log detail; --no-color to disable ANSI colors.\n");
  printf("  - Use --state DIR to persist device memory across runs (files: DIR/dev_XX.bin).\n");
  printf("  - Use --chunk N to change frame payload size at runtime (1..%d; default %d).\n", MAX_DATA_LEN, MAX_DATA_LEN);
}

/* ---------- simple persistence helpers (POSIX) ---------- */

static int ensure_dir(const char *dir)
{
  if (!dir || !*dir)
    return 0;
  struct stat st;
  if (stat(dir, &st) == 0)
  {
    return S_ISDIR(st.st_mode) ? 1 : 0;
  }
  if (mkdir(dir, 0777) == 0)
    return 1;
  if (errno == EEXIST)
    return 1;
  perror("mkdir --state DIR");
  return 0;
}

static void path_for(char *out, size_t cap, const char *dir, int addr)
{
  snprintf(out, cap, "%s/dev_%02d.bin", dir, addr);
}

static void load_state_for_device(Receiver *r, const char *dir)
{
  char path[512];
  path_for(path, sizeof(path), dir, r->addr);
  FILE *f = fopen(path, "rb");
  if (!f)
    return; // no prior state
  size_t n = fread(r->memory, 1, SLAVE_MEM_SIZE, f);
  (void)n; // partial is fine
  fclose(f);
}

static void save_state_for_device(const Receiver *r, const char *dir)
{
  char path[512];
  path_for(path, sizeof(path), dir, r->addr);
  FILE *f = fopen(path, "wb");
  if (!f)
  {
    perror("fopen --state");
    return;
  }
  fwrite(r->memory, 1, SLAVE_MEM_SIZE, f);
  fclose(f);
}

/* -------------------------------------------------------- */

int main(int argc, char **argv)
{
  int num_devices = 0;
  int have_write = 0, have_read = 0;
  int dest = -1;
  int retries = DEFAULT_MAX_RETRIES;
  double noise = DEFAULT_NOISE_PROB;
  int dump_after = 0;
  int have_addr = 0;
  unsigned long addr = 0;
  const char *data_str = NULL;
  unsigned long read_len = 0;
  const char *state_dir = NULL; // NEW
  unsigned long chunk_arg = 0;  // NEW: --chunk size (optional)

  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "--devices") && i + 1 < argc)
    {
      num_devices = atoi(argv[++i]);
    }
    else if (!strcmp(argv[i], "--dest") && i + 1 < argc)
    {
      dest = atoi(argv[++i]);
    }
    else if (!strcmp(argv[i], "--write"))
    {
      have_write = 1;
    }
    else if (!strcmp(argv[i], "--read"))
    {
      have_read = 1;
    }
    else if (!strcmp(argv[i], "--addr") && i + 1 < argc)
    {
      have_addr = 1;
      addr = strtoul(argv[++i], NULL, 0);
    }
    else if (!strcmp(argv[i], "--data") && i + 1 < argc)
    {
      data_str = argv[++i];
    }
    else if (!strcmp(argv[i], "--len") && i + 1 < argc)
    {
      read_len = strtoul(argv[++i], NULL, 0);
    }
    else if (!strcmp(argv[i], "--retries") && i + 1 < argc)
    {
      retries = atoi(argv[++i]);
      if (retries < 0)
        retries = 0;
    }
    else if (!strcmp(argv[i], "--noise") && i + 1 < argc)
    {
      noise = atof(argv[++i]);
      if (noise < 0.0)
        noise = 0.0;
      if (noise > 1.0)
        noise = 1.0;
    }
    else if (!strcmp(argv[i], "--dump"))
    {
      dump_after = 1;
    }
    else if (!strcmp(argv[i], "--state") && i + 1 < argc)
    { // NEW
      state_dir = argv[++i];
    }
    else if (!strcmp(argv[i], "--chunk") && i + 1 < argc)
    {
      chunk_arg = strtoul(argv[++i], NULL, 0);
      if (chunk_arg < 1)
        chunk_arg = 1;
      if (chunk_arg > MAX_DATA_LEN)
        chunk_arg = MAX_DATA_LEN;
    }
    else if (!strcmp(argv[i], "--verbose"))
    {
      log_set_level(LOG_LEVEL_VERBOSE);
    }
    else if (!strcmp(argv[i], "--quiet"))
    {
      log_set_level(LOG_LEVEL_QUIET);
    }
    else if (!strcmp(argv[i], "--no-color"))
    {
      log_enable_color(0);
    }
    else if (!strcmp(argv[i], "--help"))
    {
      usage(argv[0]);
      return 0;
    }
    else
    {
      printf("Unknown or incomplete argument: %s\n", argv[i]);
      usage(argv[0]);
      return 1;
    }
  }

  if (num_devices <= 0 || num_devices > MAX_DEVICES_LIMIT)
  {
    printf("Enter number of devices (1..%d): ", MAX_DEVICES_LIMIT);
    if (scanf("%d", &num_devices) != 1 || num_devices <= 0 || num_devices > MAX_DEVICES_LIMIT)
    {
      fprintf(stderr, "Invalid number of devices.\n");
      return 1;
    }
  }
  if (dest < 1 || dest > num_devices)
  {
    printf("Enter destination device address (1..%d): ", num_devices);
    if (scanf("%d", &dest) != 1 || dest < 1 || dest > num_devices)
    {
      fprintf(stderr, "Invalid destination address.\n");
      return 1;
    }
  }
  if (!have_addr)
  {
    printf("Enter memory address (0..%d): ", SLAVE_MEM_SIZE - 1);
    if (scanf("%lu", &addr) != 1 || addr >= SLAVE_MEM_SIZE)
    {
      fprintf(stderr, "Invalid memory address.\n");
      return 1;
    }
  }
  if (!have_write && !have_read)
  {
    printf("Choose operation (w=write, r=read): ");
    char ch = 0;
    while (ch != 'w' && ch != 'r')
    {
      if (scanf(" %c", &ch) != 1)
      {
        fprintf(stderr, "Invalid input.\n");
        return 1;
      }
    }
    have_write = (ch == 'w');
    have_read = (ch == 'r');
  }
  if (have_write && !data_str)
  {
    printf("Enter string to WRITE: ");
    char buf[8192];
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
    if (!fgets(buf, sizeof(buf), stdin))
    {
      fprintf(stderr, "Failed to read string.\n");
      return 1;
    }
    size_t blen = strlen(buf);
    if (blen > 0 && buf[blen - 1] == '\n')
      buf[--blen] = '\0';
    data_str = strdup(buf);
  }
  if (have_read && read_len == 0)
  {
    printf("Enter number of bytes to READ (1..%d): ", MAX_DATA_LEN);
    if (scanf("%lu", &read_len) != 1 || read_len < 1 || read_len > MAX_DATA_LEN)
    {
      fprintf(stderr, "Invalid read length.\n");
      return 1;
    }
  }

  // Apply runtime chunk size if provided
  if (chunk_arg > 0)
  {
    sender_set_chunk_size((size_t)chunk_arg);
  }

  // Prepare persistence directory if requested
  if (state_dir)
  {
    if (!ensure_dir(state_dir))
    {
      fprintf(stderr, "Could not create/access --state directory: %s\n", state_dir);
      return 1;
    }
  }

  // Create devices and bus
  Bus bus;
  bus_init(&bus, noise);

  Receiver *receivers = (Receiver *)malloc(sizeof(Receiver) * (num_devices + 1));
  if (!receivers)
  {
    fprintf(stderr, "Allocation failed.\n");
    return 1;
  }
  for (int i = 1; i <= num_devices; ++i)
  {
    receiver_init(&receivers[i], (uint8_t)i);
    if (state_dir)
    {
      load_state_for_device(&receivers[i], state_dir); // NEW
    }
    bus_attach(&bus, &receivers[i]);
  }

  int ok = 0;
  if (have_write)
  {
    log_draw_rule();
    LOGI("MASTER", "WRITE: \"%s\" -> device %d at 0x%04lX (retries=%d, noise=%.3f)\n",
         data_str, dest, addr, retries, noise);
    ok = master_write(&bus, (uint8_t)dest, (uint16_t)addr,
                      (const uint8_t *)data_str, strlen(data_str), retries);
    LOGI("MASTER", "WRITE %s\n", ok ? "SUCCESS" : "FAILED");
    log_draw_rule();
  }
  else if (have_read)
  {
    log_draw_rule();
    LOGI("MASTER", "READ: device %d at 0x%04lX for %lu bytes (retries=%d, noise=%.3f)\n",
         dest, addr, read_len, retries, noise);
    uint8_t *buf = (uint8_t *)malloc(read_len + 1);
    if (!buf)
    {
      fprintf(stderr, "Allocation failed.\n");
      free(receivers);
      return 1;
    }
    ok = master_read(&bus, (uint8_t)dest, (uint16_t)addr, buf, read_len, retries);
    if (ok)
    {
      buf[read_len] = '\0';
      LOGI("MASTER", "READ SUCCESS, data: \"%s\"\n", buf);
    }
    else
    {
      LOGE("MASTER", "READ FAILED\n");
    }
    log_draw_rule();
    free(buf);
  }

  // Save state if requested
  if (state_dir)
  {
    for (int i = 1; i <= num_devices; ++i)
    {
      save_state_for_device(&receivers[i], state_dir); // NEW
    }
  }

  if (dump_after)
  {
    for (int i = 1; i <= num_devices; ++i)
    {
      receiver_dump_memory(&receivers[i], 128);
    }
  }

  free(receivers);
  return ok ? 0 : 2;
}