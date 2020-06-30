
#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdbool.h>
#include <stdint.h>

void socket( bool enable_mac_filter );
void close( void );
uint16_t sendto( uint8_t *buf, uint16_t len );
uint16_t recv_header( void );
uint16_t recvfrom( uint8_t *buf );

#endif
