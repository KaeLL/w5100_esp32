
#pragma once

#include "esp_netif_types.h"
#include "eth-w5100-cfg.h"

struct eth_static_ip
{
	esp_netif_ip_info_t net;
	esp_ip4_addr_t p_dns;
	esp_ip4_addr_t s_dns;
	esp_ip4_addr_t f_dns;
};

struct eth_ifconfig
{
	char hostname[ 32 ];	   // Max 32 characters
	struct eth_static_ip sip;  // Data is assumed to be in network order.
	struct w5100_config_t w5100_cfg;
};

void eth_init( const struct eth_ifconfig *const cfg );
void eth_deinit( void );
