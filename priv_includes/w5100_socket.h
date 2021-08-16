
#pragma once

void w5100_socket_open( void );
void w5100_socket_close( void );
uint16_t w5100_socket_recv( uint8_t **const buf );
uint16_t w5100_socket_send( const uint8_t *const buf, const uint16_t len );
