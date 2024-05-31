
#pragma once

#include <stdint.h>

struct w5100_config_t
{
	void ( *init )( void );
	void ( *deinit )( void );
	void ( *ll_hw_reset )( void );
	void ( *read )( const uint16_t addr, uint8_t *const data_rx, const uint32_t size );
	void ( *write )( const uint16_t addr, const uint8_t *const data_tx, const uint32_t size );
};
