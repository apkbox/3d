
/** \file
  \brief SPI subsystem

  This is much simpler than Teacup's serial subsystem. No ring buffers, no
  "write string" functions. Usually using SPI directly is faster than fiddling
  with buffers. For example, reading or writing a byte can be done in as few
  as 20 clock cycles.

  Other than serial, SPI has to deal with multiple devices. Device selection
  happens before reading and writing, data exchange its self is the same for
  each device, then.
*/
#include "spi.h"

#define TEACUP_C_INCLUDE
#include "spi-avr.c"
#include "spi-arm.c"
#undef TEACUP_C_INCLUDE

/* No common code so far. */
