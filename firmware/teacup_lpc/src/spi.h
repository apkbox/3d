#ifndef _SPI_H
#define _SPI_H

#include "config_wrapper.h"
#include "arduino.h"
#include "pinio.h"

#ifdef SPI

/**
  Test configuration.
*/
#if defined __ARMEL__ && (defined TEMP_MAX6675 || defined TEMP_MCP3008)
  #error SPI (TEMP_MAX6675, TEMP_MCP3008) not yet supported on ARM.
#endif

// Uncomment this to double SPI frequency from (F_CPU / 4) to (F_CPU / 2).
//#define SPI_2X

/** Initialise SPI subsystem.
*/
void spi_init(void);

/** SPI device selection.

  Because out famous WRITE() macro works with constant pins, only, we define
  a (de)select function for each of them. In case you add another SPI device,
  you also have to define a pair of these functions.
*/
#ifdef SD
static void spi_select_sd(void) __attribute__ ((always_inline));
inline void spi_select_sd(void) {
  WRITE(SD_CARD_SELECT_PIN, 0);
}

static void spi_deselect_sd(void) __attribute__ ((always_inline));
inline void spi_deselect_sd(void) {
  WRITE(SD_CARD_SELECT_PIN, 1);
}
#endif /* SD */

#ifdef TEMP_MAX6675
// Note: the pin choosen with DEFINE_TEMP_SENSOR() in the board configuration
//       should be used here. Currently it's a requirement that this device's
//       Chip Select pin is actually SS, while any other pin would work just
//       as fine.
static void spi_select_max6675(void) __attribute__ ((always_inline));
inline void spi_select_max6675(void) {
  WRITE(SS, 0);
}

static void spi_deselect_max6675(void) __attribute__ ((always_inline));
inline void spi_deselect_max6675(void) {
  WRITE(SS, 1);
}
#endif /* TEMP_MAX6675 */

#ifdef TEMP_MCP3008
static void spi_select_mcp3008(void) __attribute__ ((always_inline));
inline void spi_select_mcp3008(void) {
  WRITE(MCP3008_SELECT_PIN, 0);
}

static void spi_deselect_mcp3008(void) __attribute__ ((always_inline));
inline void spi_deselect_mcp3008(void) {
  WRITE(MCP3008_SELECT_PIN, 1);
}
#endif /* TEMP_MCP3008 */

/** Set SPI clock speed to something between 100 and 400 kHz. */
inline void spi_speed_100_400(void);

/** Set SPI clock speed to maximum.
*/
inline void spi_speed_max(void);

/** Exchange a byte over SPI. */
inline uint8_t spi_rw(uint8_t);

#endif /* SPI */

#endif /* _SPI_H */
