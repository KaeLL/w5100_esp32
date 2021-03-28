
#pragma once

#include <stdint.h>

#include "w5100_config.h"

void w5100_spi_init( const struct w5100_config_t *const w5100_cfg );
void w5100_spi_deinit( void );
void w5100_ll_hw_reset( void );
void w5100_spi_op( const uint32_t tx, uint32_t *const rx );
