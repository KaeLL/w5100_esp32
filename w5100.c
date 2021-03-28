
#include "w5100_internal.h"
#include "w5100_ll.h"
#include "w5100.h"

void w5100_init( void )
{
	w5100_internal_init();
}

_Bool w5100_wasDataReceiceved( void )
{
	return ( getS0_IR() & S0_IR_RECV ) == S0_IR_RECV;
}

void w5100_enablePromiscuousMode( void )
{
	IINCHIP_WRITE( S0_MR, IINCHIP_READ( S0_MR ) | S0_MR_MF );
}

void w5100_disablePromiscuousMode( void )
{
	IINCHIP_WRITE( S0_MR, IINCHIP_READ( S0_MR ) & ~S0_MR_MF );
}
