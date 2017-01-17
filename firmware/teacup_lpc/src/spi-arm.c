
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

  // Use manually driven CS/SSEL
  SET_OUTPUT(SD_CARD_SELECT_PIN);
  WRITE(SD_CARD_SELECT_PIN, 1);
  //LPC_IOCON->SSEL_CMSIS = 0x01 << 0;  // Function SSEL0.

  /* Reset SPI block */
  LPC_SYSCON->PRESETCTRL &= ~(1 << 0);
  LPC_SYSCON->PRESETCTRL |= (1 << 0); /* Reset SPI block */

  // Turn on SPI/SSP power.
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);

  LPC_SYSCON->SSP0CLKDIV = 2;

  //LPC_SSP0->IMSC = 0; /* Disable interrupts */
  //LPC_SSP0->ICR = 0; /* Clear pending interrupts */

  LPC_SSP0->CR0 = (0x7 << 0)  /* DSS = 8 bit */
                | (0x0 << 4)  /* FRF = SPI */
                | (0 << 6)    /* CPOL = 0 */
                | (0 << 7)    /* CPHA = 0 */
                | (9 << 8);   /* SCR = 39 (Serial clock rate) */

  LPC_SSP0->CPSR = 0x2; /* Prescaler */

  for ( i = 0; i < 8; i++ )
  {
    int Dummy = LPC_SSP0->DR;   /* clear the RxFIFO */
  }

  LPC_SSP0->CR1 = 0x02;

  // This sets the whole SPRC register.
  spi_speed_100_400();
}

/** Set SPI clock speed to something between 100 and 400 kHz.
*/
inline void spi_speed_100_400(void) {
  LPC_SSP0->CPSR = 8; // 300kHz
}

/** Set SPI clock speed to maximum.
*/
inline void spi_speed_max(void) {
  LPC_SSP0->CPSR = 2; // 1.2Mhz
}

/** Exchange a byte over SPI.
*/
inline uint8_t spi_rw(uint8_t byte) {
  while((LPC_SSP0->SR & 0x2) == 0) {
    /* Wait until TNF is set */
  }

  LPC_SSP0->DR = byte;

  while((LPC_SSP0->SR & 0x14) != 0x4) {
    /* Wait until RNE is set */
  }

  return LPC_SSP0->DR;
}

#endif /* SPI */

#endif /* defined TEACUP_C_INCLUDE && defined __ARM__ */
