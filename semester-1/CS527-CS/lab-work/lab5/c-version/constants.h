#pragma once
#include <stdint.h>
#include <stddef.h>

/* ----- Device address map (per Lab-5 PDF) ----- */
#define KEYBOARD_BASE       0xFFFF0000u
#define KEYBOARD_MEM_BYTES  128u

#define DISPLAY_BASE        0xFFFF0100u
#define DISPLAY_MEM_BYTES   512u          /* addresses [0xFFFF0100 .. 0xFFFF02FF] */

#define ALARM_ADDR          0xFFFF0300u   /* 1 byte */
#define ALARM_MEM_BYTES     1u

#define BUTTON_ADDR         0xFFFF0301u   /* 1 byte */
#define BUTTON_MEM_BYTES    1u

/* Packet fields (protocol same as Lab-4) */
typedef enum { PKT_DATA = 0, PKT_ACK = 1 } packet_type_t;
typedef enum { REQ_READ = 0, REQ_WRITE = 1 } request_type_t;

/* Protocol limits */
#define FRAME_DATA_MAX      255u

/* Utility min/max */
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
