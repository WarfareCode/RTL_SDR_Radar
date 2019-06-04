#ifndef CONST_H
#define CONST_H

#include <stdint.h>

const uint64_t MODES_DEFAULT_RATE       = 2000000;
const uint64_t MODES_DEFAULT_FREQ       = 1090000000;

const uint16_t MODES_ASYNC_BUF_NUMBER   = 12;
constexpr uint64_t MODES_DATA_LEN       = (16*16384);   /* 256k */
const int32_t  MODES_AUTO_GAIN          = -100;         /* Use automatic gain. */
const int32_t  MODES_MAX_GAIN           = 999999;       /* Use max available gain. */

const uint8_t  MODES_PREAMBLE_US        = 8;            /* microseconds */
const uint16_t MODES_LONG_MSG_BITS      = 112;
const uint16_t MODES_SHORT_MSG_BITS     = 56;

constexpr uint32_t MODES_FULL_LEN           = (MODES_PREAMBLE_US + MODES_LONG_MSG_BITS);
constexpr uint32_t MODES_FULL_LEN_OFFS      = ((MODES_PREAMBLE_US+MODES_LONG_MSG_BITS) - 1) * 4;
constexpr uint16_t MODES_LONG_MSG_BYTES     = (MODES_LONG_MSG_BITS / MODES_PREAMBLE_US);
constexpr uint16_t MODES_SHORT_MSG_BYTES    = (MODES_SHORT_MSG_BITS / MODES_PREAMBLE_US);

const uint16_t MODES_ICAO_CACHE_LEN     = 1024; /* Power of two required. */
const uint16_t MODES_ICAO_CACHE_TTL     = 60;   /* Time to live of cached addresses. */
const int8_t MODES_UNIT_FEET            = 0;
const int8_t MODES_UNIT_METERS          = 1;

const uint8_t MODES_DEBUG_DEMOD         = (1<<0);
const uint8_t MODES_DEBUG_DEMODERR      = (1<<1);
const uint8_t MODES_DEBUG_BADCRC        = (1<<2);
const uint8_t MODES_DEBUG_GOODCRC       = (1<<3);
const uint8_t MODES_DEBUG_NOPREAMBLE    = (1<<4);


/* When debug is set to MODES_DEBUG_NOPREAMBLE, the first sample must be
 * at least greater than a given level for us to dump the signal. */
const uint16_t MODES_DEBUG_NOPREAMBLE_LEVEL  = 25;

const uint16_t MODES_INTERACTIVE_ROWS   = 15;   /* Rows on screen */
const uint16_t MODES_INTERACTIVE_TTL    = 6000;  /* TTL before being removed in ms*/

#endif // CONST_H

