
#pragma once

struct w5100_config_t
{
	void ( *init )( void );
	void ( *deinit )( void );
	void ( *ll_hw_reset )( void );
	int ( *read )( const uint16_t addr, uint8_t *const data_rx, const uint32_t size );
	int ( *write )( const uint16_t addr, const uint8_t *const data_tx, const uint32_t size );
};