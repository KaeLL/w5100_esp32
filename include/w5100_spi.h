
#pragma once

#include "sdkconfig.h"

#include <stdint.h>

#ifdef CONFIG_W5100_CUSTOM_SPI_TRANS
#	include "driver/spi_master.h"

typedef void ( *spi_cb_t )( spi_device_handle_t spi, uint32_t buf_w, uint32_t *buf_r );

void set_spi_trans_cb( spi_cb_t spi_cb );
#endif

void w5100_spi_init( void );

void w5100_spi_deinit( void );

void w5100_spi_op( uint32_t tx, uint32_t *rx );
