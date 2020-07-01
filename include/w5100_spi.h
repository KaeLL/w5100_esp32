
#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

void w5100_spi_init( void );

void w5100_transaction( uint32_t *w_b, uint32_t *r_b, uint16_t sz );

void spi_transaction( uint32_t tx, uint32_t *rx );

#endif