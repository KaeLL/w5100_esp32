
#include <assert.h>
#include <stdlib.h>

#include "w5100_debug.h"
#include "w5100.h"
#include "w5100_socket.h"

tag_def( "w5100_socket" );

void w5100_socket( bool enable_mac_filter )
{
	f_entry();
	do
	{
		w5100_close();
		IINCHIP_WRITE( S0_MR, enable_mac_filter ? S0_MR_MACRAW | S0_MR_MF : S0_MR_MACRAW );
		IINCHIP_WRITE( S0_CR, S0_CR_OPEN );

		while ( IINCHIP_READ( S0_CR ) )
			;
	} while ( IINCHIP_READ( S0_SR ) != SOCK_MACRAW );
	f_exit();
}

void w5100_close( void )
{
	f_entry();
	IINCHIP_WRITE( S0_CR, S0_CR_CLOSE );

	while ( IINCHIP_READ( S0_CR ) )
		;

	IINCHIP_WRITE( S0_IR, 0xFF );
	f_exit();
}

uint16_t w5100_send( uint8_t *buf, uint16_t len )
{
	f_entry();
	while ( len > getS0_XX_XSR( S0_TX_FSR0 ) )
		;

	send_data_processing( buf, len );

	IINCHIP_WRITE( S0_CR, S0_CR_SEND );

	while ( IINCHIP_READ( S0_CR ) )
		;

	uint8_t ir_reg;
	while ( !( ( ir_reg = IINCHIP_READ( S0_IR ) ) & S0_IR_SEND_OK ) )
		if ( ir_reg & S0_IR_TIMEOUT )
		{
			f_exit();
			return 0;
		}
	f_exit();

	return len;
}

uint16_t w5100_recv( uint8_t **buf )
{
	f_entry();
	uint16_t data_len, ptr = getS0_XX_XSR( S0_RX_RD0 );

	read_data( ptr, ( uint8_t * )&data_len, 2 );
	ptr += 2;
	data_len = __builtin_bswap16( data_len ) - 2;

	if ( !( *buf = malloc( data_len ) ) )
	{
		f_exit();
		return 0;
	}

	read_data( ptr, *buf, data_len );
	write_uint16_reg( S0_RX_RD0, ptr + data_len );

	IINCHIP_WRITE( S0_CR, S0_CR_RECV );

	while ( IINCHIP_READ( S0_CR ) )
		;
	f_exit();

	return data_len;
}
