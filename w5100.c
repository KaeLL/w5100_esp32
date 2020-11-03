
#include <stdio.h>
#include <string.h>

#include "w5100_debug.h"
#include "w5100.h"
#include "w5100_ll.h"

#define W_PCK( address, data ) ( __builtin_bswap32( ( 0xF0000000 | ( address ) << 8 | ( data ) ) ) )
#define R_PCK( address )	   ( __builtin_bswap32( ( 0x0F000000 | ( address ) << 8 ) ) )

tag_def( "w5100" );

void iinchip_init( void )
{
	f_entry();
	w5100_ll_hw_reset();
	IINCHIP_WRITE( MR0, MR_RST );

	while ( IINCHIP_READ( MR0 ) )
		;

	IINCHIP_WRITE( TMSR, 3 );
	IINCHIP_WRITE( RMSR, 3 );
	f_exit();
}

void IINCHIP_WRITE( uint16_t addr, uint8_t data )
{
	uint32_t tx = W_PCK( addr, data );

	w5100_spi_op( tx, NULL );
}

uint8_t IINCHIP_READ( uint16_t addr )
{
	uint32_t data, tx = R_PCK( addr );

	w5100_spi_op( tx, &data );

	return data >> 24;
}

void wiz_write_buf( uint16_t addr, uint8_t *buf, uint16_t len )
{
	for ( uint16_t i = 0; i < len; ++i )
		IINCHIP_WRITE( addr + i, buf[ i ] );
}

void wiz_read_buf( uint16_t addr, uint8_t *buf, uint16_t len )
{
	for ( uint16_t i = 0; i < len; ++i )
		buf[ i ] = IINCHIP_READ( addr + i );
}

uint16_t read_uint16_reg( uint16_t addr )
{
	uint16_t reg;

	wiz_read_buf( addr, ( uint8_t * )&reg, 2 );

	return __builtin_bswap16( reg );
}

void write_uint16_reg( uint16_t addr, uint16_t data )
{
	wiz_write_buf( addr, ( uint8_t * )&( uint16_t ){ __builtin_bswap16( data ) }, 2 );
}

uint16_t getS0_XX_XSR( uint16_t addr )
{
	f_entry();
	uint16_t first_read, second_read;

	do
		wiz_read_buf( addr, ( uint8_t * )&first_read, 2 ), wiz_read_buf( addr, ( uint8_t * )&second_read, 2 );
	while ( first_read != second_read );
	f_exit();

	return __builtin_bswap16( first_read );
}

uint8_t getS0_IR( void )
{
	uint8_t ir_reg;

	if ( !( ir_reg = IINCHIP_READ( S0_IR ) ) )
		return 0;

	IINCHIP_WRITE( S0_IR, ir_reg );

	return ir_reg;
}

void send_data_processing( uint8_t *data, uint16_t len )
{
	uint16_t ptr = read_uint16_reg( S0_TX_WR0 );
	uint16_t size;
	uint16_t dst_mask = ptr & SMASK;
	uint16_t dst_ptr = SBUFBASEADDRESS + dst_mask;

	if ( dst_mask + len <= SSIZE )
		wiz_write_buf( dst_ptr, data, len );
	else
	{
		size = SSIZE - dst_mask;
		wiz_write_buf( dst_ptr, data, size );
		data += size;
		size = len - size;
		wiz_write_buf( SBUFBASEADDRESS, data, size );
	}

	write_uint16_reg( S0_TX_WR0, ptr + len );
}

void read_data( uint16_t src_addr, uint8_t *dst, uint16_t len )
{
	uint16_t src_mask = src_addr & RMASK;
	uint16_t real_src_addr = RBUFBASEADDRESS + src_mask;

	if ( src_mask + len <= RSIZE )
		wiz_read_buf( real_src_addr, dst, len );
	else
	{
		uint16_t size = RSIZE - src_mask;
		wiz_read_buf( real_src_addr, dst, size );
		dst += size;
		size = len - size;
		wiz_read_buf( RBUFBASEADDRESS, dst, size );
	}
}
