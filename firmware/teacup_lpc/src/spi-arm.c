
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
#if defined TEACUP_C_INCLUDE && defined __ARMEL__

#include "arduino.h"
#include "cmsis-lpc11xx.h"
#include "config_wrapper.h"

#ifdef SPI

/** Initialise SPI subsystem.
*/
void spi_init() {
  int i;

  // Enable clock for IOCON
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);

  LPC_IOCON->SCK_CMSIS = 0x01 << 0;  // Function SCK0.
  LPC_IOCON->SCK_LOC = 1;     /* SCK0 on PIO2_11 */
  LPC_IOCON->MISO_CMSIS = 0x01 << 0;  // Function MISO0.
  LPC_IOCON->MOSI_CMSIS = 0x01 << 0;  // Function MOSI0.
  LPC_IOCON->SSEL_CMSIS = 0x01 << 0;  // Function SSEL0.

  // Turn on SPI/SSP power.
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);

  LPC_SYSCON->SSP0CLKDIV = 2;

  LPC_SYSCON->PRESETCTRL |= (1 << 0); /* Reset SPI block */

  LPC_SSP0->IMSC = 0; /* Disable interrupts */
  LPC_SSP0->ICR = 0; /* Clear pending interrupts */

  LPC_SSP0->CR0 = (0x7 << 0)  /* DSS = 8 bit */
                | (0x0 << 4)  /* FRF = SPI */
                | (0 << 6)    /* CPOL = 0 */
                | (0 << 7)    /* CPHA = 0 */
                | (0 << 8);   /* SCR = 35 (Serial clock rate) */
  LPC_SSP1->CR1 = (0 << 0) /* LBM = 0 Loopback mode */
                | (1 << 1) /* SSE = 1 enable SPI */
                | (0 << 2); /* MS = 0 (Master) */

  LPC_SSP0->CPSR = 0x2; /* Prescaler */

  for ( i = 0; i < 8; i++ )
  {
    int Dummy = LPC_SSP0->DR;   /* clear the RxFIFO */
  }

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
inline void spi_speed_100_400(void) {
  // SPCR = 0x53;
}

/** Set SPI clock speed to maximum.
*/
inline void spi_speed_max(void) {
  // SPCR = 0x50; // See list at spi_speed_100_400().
}

/** Exchange a byte over SPI.
*/
inline uint8_t spi_rw(uint8_t byte) {
  while((LPC_SSP0->SR & 0x2) == 0) {
    /* Wait until TNF is set */
  }

  LPC_SSP0->DR = byte;

  while((LPC_SSP0->SR & 0x4) == 0) {
    /* Wait until RNE is set */
  }

  return LPC_SSP0->DR;
}

#endif /* SPI */

#endif /* defined TEACUP_C_INCLUDE && defined __ARM__ */
