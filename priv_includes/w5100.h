
#pragma once

#include <stdbool.h>

void w5100_init( void );
bool w5100_wasDataReceiceved( void );
void w5100_enablePromiscuousMode( void );
void w5100_disablePromiscuousMode( void );
void w5100_setMAC( uint8_t *mac );