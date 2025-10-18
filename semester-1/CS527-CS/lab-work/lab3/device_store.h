#pragma once
#include <stddef.h>
#include <stdint.h>

/* Load/save the device (receiver) memory to a small binary file.
  If 'path' is NULL or empty, implementation falls back to defaults. */

/* Returns 0 on success (or file-not-found on load); non-zero on I/O error. */
int device_store_load(uint8_t* mem, size_t n, const char* path);

/* Returns 0 on success; non-zero on I/O error. */
int device_store_save(const uint8_t* mem, size_t n, const char* path);
