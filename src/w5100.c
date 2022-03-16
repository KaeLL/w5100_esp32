
#include <stdbool.h>

#include "w5100_internal.h"
#include "w5100.h"
#include "w5100_reg.h"

static uint8_t getS0_IR( void )
{
	uint8_t ir_reg;

	if ( !( ir_reg = wiz_r_byte( S0_IR ) ) )
		return 0;

	wiz_w_byte( S0_IR, ir_reg );

	return ir_reg;
}

void w5100_init( void )
{
	w5100_internal_init();
	wiz_w_byte( MR0, MR_RST );

	while ( wiz_r_byte( MR0 ) )
		;

	wiz_w_byte( RMSR, 3 );
	wiz_w_byte( TMSR, 3 );
}

bool w5100_wasDataReceiceved( void )
{
	return ( getS0_IR() & S0_IR_RECV ) == S0_IR_RECV;
}

void w5100_enablePromiscuousMode( void )
{
	wiz_w_byte( S0_MR, wiz_r_byte( S0_MR ) & ~S0_MR_MF );
}

void w5100_disablePromiscuousMode( void )
{
	wiz_w_byte( S0_MR, wiz_r_byte( S0_MR ) | S0_MR_MF );
}

void w5100_setMAC( uint8_t *mac )
{
	wiz_w_cont( SHAR0, mac, 6 );
}