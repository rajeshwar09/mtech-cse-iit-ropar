#pragma once
#include <stdint.h>
#include <stddef.h>

/* CRC-16/CCITT-FALSE over data bytes only */
uint16_t crc16_ccitt_false(const uint8_t* data, size_t len);
