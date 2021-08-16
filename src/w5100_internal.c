
#include <stdio.h>
#include <string.h>

#include "w5100_config.h"
#include "w5100_reg.h"
#include "w5100_internal.h"

static const struct w5100_config_t *w5100_cfg;

void pre_init_setup( void *arg )
{
	w5100_cfg = ( const struct w5100_config_t * )arg;
}

void w5100_internal_init( void )
{
	w5100_cfg->ll_hw_reset();
}

void wiz_w_byte( const uint16_t addr, const uint8_t data )
{
	w5100_cfg->write( addr, &data, 1 );
}

uint8_t wiz_r_byte( const uint16_t addr )
{
	uint8_t data;
	w5100_cfg->read( addr, &data, 1 );
	return data;
}

void wiz_w_cont( const uint16_t addr, const uint8_t *const buf, const uint16_t len )
{
	w5100_cfg->write( addr, buf, len );
}

void wiz_r_cont( const uint16_t addr, uint8_t *const buf, const uint16_t len )
{
	w5100_cfg->read( addr, buf, len );
}
