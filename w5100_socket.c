
#include <assert.h>
#include <stdlib.h>

#include "sys/param.h"

#include "w5100_internal.h"
#include "w5100_socket.h"

void w5100_socket_open( const _Bool enable_mac_filter )
{
	do
	{
		w5100_socket_close();
		IINCHIP_WRITE( S0_MR, enable_mac_filter ? S0_MR_MACRAW | S0_MR_MF : S0_MR_MACRAW );
		IINCHIP_WRITE( S0_CR, S0_CR_OPEN );

		while ( IINCHIP_READ( S0_CR ) )
			;
	} while ( IINCHIP_READ( S0_SR ) != SOCK_MACRAW );
}

void w5100_socket_close( void )
{
	do
		IINCHIP_WRITE( S0_CR, S0_CR_CLOSE );
	while ( IINCHIP_READ( S0_CR ) );

	do
		IINCHIP_WRITE( S0_IR, 0xFF );
	while ( IINCHIP_READ( S0_IR ) );
}

uint16_t w5100_socket_send( const uint8_t *const buf, const uint16_t len )
{
	uint16_t data_remaining = len;

	while ( data_remaining )
	{
		uint16_t avail_sz = getS0_XX_XSR( S0_TX_FSR0 );

		if ( !avail_sz )
			continue;

		uint16_t sz_to_be_sent = MIN( avail_sz, data_remaining );

		send_data_processing( buf + len - data_remaining, sz_to_be_sent );

		IINCHIP_WRITE( S0_CR, S0_CR_SEND );

		while ( IINCHIP_READ( S0_CR ) )
			;

		while ( !( IINCHIP_READ( S0_IR ) & S0_IR_SEND_OK ) )
			;

		data_remaining -= sz_to_be_sent;
	}

	return len;
}

uint16_t w5100_socket_recv( uint8_t **const buf )
{
	uint16_t data_len, ptr = read_uint16_reg( S0_RX_RD0 );

	read_data( ptr, ( uint8_t * )&data_len, 2 );
	ptr += 2;
	data_len = __builtin_bswap16( data_len ) - 2;

	if ( !( *buf = malloc( data_len ) ) )
	{
		return 0;
	}

	read_data( ptr, *buf, data_len );
	write_uint16_reg( S0_RX_RD0, ptr + data_len );

	IINCHIP_WRITE( S0_CR, S0_CR_RECV );

	while ( IINCHIP_READ( S0_CR ) )
		;

	return data_len;
}
