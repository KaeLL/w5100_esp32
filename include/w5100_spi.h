
#pragma once

#include <stdint.h>

void w5100_spi_init( void );

void w5100_spi_op( uint32_t tx, uint32_t *rx );
