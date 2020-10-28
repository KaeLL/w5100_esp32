
#pragma once

#include <stdbool.h>
#include <stdint.h>

void w5100_socket( bool enable_mac_filter );
void w5100_close( void );
uint16_t w5100_recv( uint8_t **buf );
uint16_t w5100_send( uint8_t *buf, uint16_t len );
