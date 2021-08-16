
#pragma once

#include <stdint.h>

void pre_init_setup( void *arg );
void w5100_internal_init( void );
uint8_t wiz_r_byte( const uint16_t addr );
void wiz_w_byte( const uint16_t addr, const uint8_t data );
void wiz_r_cont( const uint16_t addr, uint8_t *const buf, const uint16_t len );
void wiz_w_cont( const uint16_t addr, const uint8_t *const buf, const uint16_t len );
