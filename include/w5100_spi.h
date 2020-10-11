
#pragma once

#include "sdkconfig.h"

#include <stdint.h>

void w5100_spi_mtx_set( void *spi_mtx );

void w5100_spi_init( void );

void w5100_spi_deinit( void );

void w5100_spi_op( uint32_t tx, uint32_t *rx );
