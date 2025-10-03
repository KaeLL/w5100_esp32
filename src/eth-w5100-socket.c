
#include "eth-w5100-socket.h"

#include "eth-w5100-hal.h"
#include "eth-w5100-reg.h"

#include "sys/param.h"

#include <stdlib.h>

static uint16_t read_uint16_reg( const uint16_t addr )
{
	uint16_t reg;

	wiz_r_cont( addr, ( uint8_t *const )&reg, 2 );

	return __builtin_bswap16( reg );
}

static void write_uint16_reg( const uint16_t addr, const uint16_t data )
{
	wiz_w_cont( addr, ( const uint8_t *const )&( uint16_t ) { __builtin_bswap16( data ) }, 2 );
}

static uint16_t getS0_XX_XSR( const uint16_t addr )
{
	uint16_t first_read, second_read;

	do
		wiz_r_cont( addr, ( uint8_t *const )&first_read, 2 ), wiz_r_cont( addr, ( uint8_t *const )&second_read, 2 );
	while ( first_read != second_read );

	return __builtin_bswap16( first_read );
}

static void send_data_processing( const uint8_t *data, const uint16_t len )
{
	uint16_t ptr = read_uint16_reg( S0_TX_WR0 );
	uint16_t size;
	uint16_t dst_mask = ptr & SMASK;
	uint16_t dst_ptr = SBUFBASEADDRESS + dst_mask;

	if ( dst_mask + len <= SSIZE )
		wiz_w_cont( dst_ptr, data, len );
	else
	{
		size = SSIZE - dst_mask;
		wiz_w_cont( dst_ptr, data, size );
		data += size;
		size = len - size;
		wiz_w_cont( SBUFBASEADDRESS, data, size );
	}

	write_uint16_reg( S0_TX_WR0, ptr + len );
}

static void read_data( const uint16_t src_addr, uint8_t *const dst, const uint16_t len )
{
	uint16_t src_mask = src_addr & RMASK;
	uint16_t real_src_addr = RBUFBASEADDRESS + src_mask;

	if ( src_mask + len <= RSIZE )
		wiz_r_cont( real_src_addr, dst, len );
	else
	{
		uint16_t size = RSIZE - src_mask;
		uint8_t *const dst_offset = dst;
		wiz_r_cont( real_src_addr, dst_offset, size );
		*( uint8_t ** )&dst_offset += size;
		size = len - size;
		wiz_r_cont( RBUFBASEADDRESS, dst_offset, size );
	}
}

void w5100_socket_open( void )
{
	do
	{
		w5100_socket_close();
		wiz_w_byte( S0_MR, S0_MR_MACRAW );
		wiz_w_byte( S0_CR, S0_CR_OPEN );

		while ( wiz_r_byte( S0_CR ) )
			;
	} while ( wiz_r_byte( S0_SR ) != SOCK_MACRAW );
}

void w5100_socket_close( void )
{
	do
		wiz_w_byte( S0_CR, S0_CR_CLOSE );
	while ( wiz_r_byte( S0_CR ) );

	do
		wiz_w_byte( S0_IR, 0xFF );
	while ( wiz_r_byte( S0_IR ) );
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

		wiz_w_byte( S0_CR, S0_CR_SEND );

		while ( wiz_r_byte( S0_CR ) )
			;

		while ( !( wiz_r_byte( S0_IR ) & S0_IR_SEND_OK ) )
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
		return 0;

	read_data( ptr, *buf, data_len );
	write_uint16_reg( S0_RX_RD0, ptr + data_len );

	wiz_w_byte( S0_CR, S0_CR_RECV );

	while ( wiz_r_byte( S0_CR ) )
		;

	return data_len;
}
