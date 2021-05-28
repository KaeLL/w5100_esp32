
#pragma once

#include "w5100_config.h"

typedef union
{
	uint32_t u32;
	uint8_t u8[ 4 ];
} dword;

struct eth_static_ip
{
	dword ip;
	dword nm;
	dword gw;
	dword p_dns;
	dword s_dns;
	dword f_dns;
};

struct eth_ifconfig
{
	char hostname[ 32 ];			 // Max 32 characters
	struct eth_static_ip sip; // Data is assumed to be in network order.
	struct w5100_config_t w5100_cfg;
};

void eth_init( const struct eth_ifconfig *const cfg );
void eth_deinit( void );