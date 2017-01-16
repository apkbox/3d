
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

#if defined TEACUP_C_INCLUDE && defined __AVR__

#include "config_wrapper.h"

#ifdef SPI

/** Initialise SPI subsystem.

  Code copied from ATmega164/324/644/1284 data sheet, section 18.2, page 160,
  or moved here from mendel.c.
*/
void spi_init() {

  // Set SCK (clock) and MOSI line to output, ie. set USART in master mode.
  SET_OUTPUT(SCK);
  SET_OUTPUT(MOSI);
  SET_INPUT(MISO);
  // SS must be set as output to disconnect it from the SPI subsystem.
  // Too bad if something else tries to use this pin as digital input.
  // See ATmega164/324/644/1284 data sheet, section 18.3.2, page 162.
  // Not written there: this must apparently be done before setting the SPRC
  // register, else future R/W-operations may hang.
  SET_OUTPUT(SS);

  #ifdef SPI_2X
    SPSR = 0x01;
  #else
    SPSR = 0x00;
  #endif

  // This sets the whole SPRC register.
  spi_speed_100_400();
}

/** Set SPI clock speed to something between 100 and 400 kHz.

  This is needed for initialising SD cards. We set the whole SPCR register
  in one step, because this is faster than and'ing in bits.

  About dividers. We have:
  SPCR = 0x50; // normal mode: (F_CPU / 4), 2x mode: (F_CPU / 2)
  SPCR = 0x51; // normal mode: (F_CPU / 16), 2x mode: (F_CPU / 8)
  SPCR = 0x52; // normal mode: (F_CPU / 64), 2x mode: (F_CPU / 32)
  SPCR = 0x53; // normal mode: (F_CPU / 128), 2x mode: (F_CPU / 64)

  For now we always choose the /128 divider, because it fits nicely in all
  expected situations:
    F_CPU                    16 MHz    20 MHz
    SPI clock normal mode   125 kHz   156 kHz
    SPI clock 2x mode       250 kHz   312 kHz

  About the other bits:
  0x50 = (1 << SPE) | (1 << MSTR);
  This is Master SPI mode, SPI enabled, interrupts disabled, polarity mode 0
  (right for SD cards).
  See ATmega164/324/644/1284 data sheet, section 18.5.1, page 164.
*/
static void spi_speed_100_400(void) __attribute__ ((always_inline));
inline void spi_speed_100_400(void) {
  SPCR = 0x53;
}

/** Set SPI clock speed to maximum.
*/
static void spi_speed_max(void) __attribute__ ((always_inline));
inline void spi_speed_max(void) {
  SPCR = 0x50; // See list at spi_speed_100_400().
}

/** Exchange a byte over SPI.

  Yes, SPI is that simple and you can always only swap bytes. To retrieve
  a byte, simply send a dummy value, like: mybyte = spi_rw(0);

  As we operate in master mode, we don't have to fear to hang due to
  communications errors (e.g. missing a clock beat).

  Note: insisting on inlinig (attribute always_inline) costs about 80 bytes
        with the current SD card code.
*/
static uint8_t spi_rw(uint8_t) __attribute__ ((always_inline));
inline uint8_t spi_rw(uint8_t byte) {
  SPDR = byte;
  loop_until_bit_is_set(SPSR, SPIF);
  return SPDR;
}

#endif /* SPI */

#endif /* defined TEACUP_C_INCLUDE && defined __AVR__ */
