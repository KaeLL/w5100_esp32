
#pragma once

#include "esp_eth_mac.h"
#include "esp_eth_phy.h"

typedef union
{
	uint32_t u32;
	uint8_t u8[4];
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
	char *hostname;				// Max 32 characters
	struct eth_static_ip sip;	// Data is assumed to be in network order.
};

esp_eth_mac_t *esp_eth_mac_new_w5100( const eth_mac_config_t *mac_config );

esp_eth_phy_t *esp_eth_phy_new_w5100( const eth_phy_config_t *config );

void eth_main( struct eth_ifconfig *cfg );
