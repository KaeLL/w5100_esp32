
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "w5100_ll.h"
#include "w5100_config.h"
#include "eth_if.h"
#include "eth_main.h"
#include "w5100_internal.h"

static const char *TAG = "eth_main";

struct
{
	esp_eth_config_t eth_config;
	esp_netif_t *eth_netif;
	esp_eth_handle_t eth_handle;
	void *eth_netif_glue;
	struct w5100_config_t w5100_cfg;
} * eth_cfgs;

void eth_enable_static_ip( const struct eth_static_ip *const sip )
{
	ESP_ERROR_CHECK( !sip );
	ESP_ERROR_CHECK( !sip->ip.u32 );
	ESP_ERROR_CHECK( !sip->nm.u32 );
	ESP_ERROR_CHECK( !sip->gw.u32 );
	ESP_ERROR_CHECK( !sip->p_dns.u32 );
	ESP_ERROR_CHECK( !sip->s_dns.u32 );

	esp_netif_ip_info_t ip_info;
	esp_netif_dhcp_status_t dhcp_status;
	esp_netif_dns_info_t dns_info;

	ESP_LOGD( TAG, "Before disabling DHCP" );
	ESP_ERROR_CHECK( esp_netif_get_old_ip_info( eth_cfgs->eth_netif, &ip_info ) );
	ESP_LOGD( TAG, "OLD_IP_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "ETHIP: " IPSTR, IP2STR( &ip_info.ip ) );
	ESP_LOGD( TAG, "ETHMASK: " IPSTR, IP2STR( &ip_info.netmask ) );
	ESP_LOGD( TAG, "ETHGW: " IPSTR, IP2STR( &ip_info.gw ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_ip_info( eth_cfgs->eth_netif, &ip_info ) );
	ESP_LOGD( TAG, "IP_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "ETHIP: " IPSTR, IP2STR( &ip_info.ip ) );
	ESP_LOGD( TAG, "ETHMASK: " IPSTR, IP2STR( &ip_info.netmask ) );
	ESP_LOGD( TAG, "ETHGW: " IPSTR, IP2STR( &ip_info.gw ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_MAIN, &dns_info ) );
	ESP_LOGD( TAG, "MAIN DNS_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "DNSIP: " IPSTR, IP2STR( &dns_info.ip.u_addr.ip4 ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_BACKUP, &dns_info ) );
	ESP_LOGD( TAG, "SECONDARY DNS_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "DNSIP: " IPSTR, IP2STR( &dns_info.ip.u_addr.ip4 ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_FALLBACK, &dns_info ) );
	ESP_LOGD( TAG, "FALLBACK DNS_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "DNSIP: " IPSTR, IP2STR( &dns_info.ip.u_addr.ip4 ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_dhcpc_get_status( eth_cfgs->eth_netif, &dhcp_status ) );
	ESP_LOGD( TAG, "DHCP STATUS: %d", dhcp_status );

	ESP_LOGD( TAG, "Attempting to stop it..." );

	ESP_ERROR_CHECK( esp_netif_dhcpc_stop( eth_cfgs->eth_netif ) );
	ESP_LOGD( TAG, "DHCP stopped. Changing IP info..." );

	ip_info.ip.addr = sip->ip.u32;
	ip_info.netmask.addr = sip->nm.u32;
	ip_info.gw.addr = sip->gw.u32;
	ESP_ERROR_CHECK( esp_netif_set_old_ip_info( eth_cfgs->eth_netif, &ip_info ) );
	ESP_ERROR_CHECK( esp_netif_set_ip_info( eth_cfgs->eth_netif, &ip_info ) );

	dns_info.ip.u_addr.ip4.addr = sip->p_dns.u32;
	ESP_ERROR_CHECK( esp_netif_set_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_MAIN, &dns_info ) );
	dns_info.ip.u_addr.ip4.addr = sip->s_dns.u32;
	ESP_ERROR_CHECK( esp_netif_set_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_BACKUP, &dns_info ) );
	if ( sip->f_dns.u32 )
	{
		dns_info.ip.u_addr.ip4.addr = sip->f_dns.u32;
		ESP_ERROR_CHECK( esp_netif_set_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_FALLBACK, &dns_info ) );
	}

	ESP_LOGD( TAG, "After static IP setup..." );
	ESP_ERROR_CHECK( esp_netif_get_old_ip_info( eth_cfgs->eth_netif, &ip_info ) );
	ESP_LOGD( TAG, "OLD_IP_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "ETHIP: " IPSTR, IP2STR( &ip_info.ip ) );
	ESP_LOGD( TAG, "ETHMASK: " IPSTR, IP2STR( &ip_info.netmask ) );
	ESP_LOGD( TAG, "ETHGW: " IPSTR, IP2STR( &ip_info.gw ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_ip_info( eth_cfgs->eth_netif, &ip_info ) );
	ESP_LOGD( TAG, "IP_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "ETHIP: " IPSTR, IP2STR( &ip_info.ip ) );
	ESP_LOGD( TAG, "ETHMASK: " IPSTR, IP2STR( &ip_info.netmask ) );
	ESP_LOGD( TAG, "ETHGW: " IPSTR, IP2STR( &ip_info.gw ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_MAIN, &dns_info ) );
	ESP_LOGD( TAG, "MAIN DNS_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "DNSIP: " IPSTR, IP2STR( &dns_info.ip.u_addr.ip4 ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_BACKUP, &dns_info ) );
	ESP_LOGD( TAG, "SECONDARY DNS_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "DNSIP: " IPSTR, IP2STR( &dns_info.ip.u_addr.ip4 ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_FALLBACK, &dns_info ) );
	ESP_LOGD( TAG, "FALLBACK DNS_INFO" );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_LOGD( TAG, "DNSIP: " IPSTR, IP2STR( &dns_info.ip.u_addr.ip4 ) );
	ESP_LOGD( TAG, "~~~~~~~~~~~" );
	ESP_ERROR_CHECK( esp_netif_dhcpc_get_status( eth_cfgs->eth_netif, &dhcp_status ) );
	ESP_LOGD( TAG, "DHCP STATUS: %d", dhcp_status );
}

void eth_deinit( void )
{
	ESP_ERROR_CHECK( esp_eth_stop( eth_cfgs->eth_handle ) );
	ESP_ERROR_CHECK( esp_eth_del_netif_glue( eth_cfgs->eth_netif_glue ) );
	ESP_ERROR_CHECK( esp_eth_clear_default_handlers( eth_cfgs->eth_netif ) );
	ESP_ERROR_CHECK( esp_eth_driver_uninstall( eth_cfgs->eth_handle ) );
	ESP_ERROR_CHECK( eth_cfgs->eth_config.phy->del( eth_cfgs->eth_config.phy ) );
	ESP_ERROR_CHECK( eth_cfgs->eth_config.mac->del( eth_cfgs->eth_config.mac ) );
	esp_netif_destroy( eth_cfgs->eth_netif );
	eth_cfgs->w5100_cfg.deinit();
	free( eth_cfgs );
}

void eth_init( const struct eth_ifconfig *const cfg )
{
	ESP_ERROR_CHECK( !( eth_cfgs = calloc( 1, sizeof *eth_cfgs ) ) );

	eth_cfgs->eth_netif = esp_netif_new( &( const esp_netif_config_t )ESP_NETIF_DEFAULT_ETH() );

	if ( cfg )
	{
		if ( *cfg->hostname )
			ESP_ERROR_CHECK( esp_netif_set_hostname( eth_cfgs->eth_netif, cfg->hostname ) );

		if ( cfg->sip.ip.u32 )
			eth_enable_static_ip( &cfg->sip );
	}

	memcpy( &eth_cfgs->w5100_cfg, &cfg->w5100_cfg, sizeof cfg->w5100_cfg );
	pre_init_setup( &eth_cfgs->w5100_cfg );

	// Set default handlers to process TCP/IP stuffs
	ESP_ERROR_CHECK( esp_eth_set_default_handlers( eth_cfgs->eth_netif ) );

	eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
	// mac_config.rx_task_stack_size = 1536;
	mac_config.smi_mdc_gpio_num = -1;  // w5100 doesn't have SMI interface
	mac_config.smi_mdio_gpio_num = -1;
	esp_eth_mac_t *mac = esp_eth_mac_new_w5100( &mac_config );
	esp_eth_phy_t *phy = esp_eth_phy_new_w5100( NULL );	 // No PHY pins connected

	esp_eth_config_t eth_cfg = ETH_DEFAULT_CONFIG( mac, phy );
	eth_cfgs->eth_config = eth_cfg;
	// eth_cfgs->eth_config.check_link_period_ms = UINT32_MAX;

	eth_cfgs->w5100_cfg.init();

	ESP_ERROR_CHECK( esp_eth_driver_install( &eth_cfgs->eth_config, &eth_cfgs->eth_handle ) );
	ESP_ERROR_CHECK( !( eth_cfgs->eth_netif_glue = esp_eth_new_netif_glue( eth_cfgs->eth_handle ) ) );

	uint8_t mac_addr[ 6 ];
	ESP_ERROR_CHECK( esp_read_mac( mac_addr, ESP_MAC_ETH ) );
	ESP_ERROR_CHECK( esp_eth_ioctl( eth_cfgs->eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr ) );

	/* attach Ethernet driver to TCP/IP stack */
	ESP_ERROR_CHECK( esp_netif_attach( eth_cfgs->eth_netif, eth_cfgs->eth_netif_glue ) );
	/* start Ethernet driver state machine */
	ESP_ERROR_CHECK( esp_eth_start( eth_cfgs->eth_handle ) );
}
