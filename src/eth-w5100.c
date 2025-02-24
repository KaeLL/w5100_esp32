
#include "eth-w5100.h"

#include "esp_eth_netif_glue.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "eth-w5100-hal.h"
#include "eth-w5100-if.h"
#include "lwip/ip4_addr.h"

static const char *const __unused TAG = "eth_main";

struct
{
	esp_eth_config_t eth_config;
	esp_netif_t *eth_netif;
	esp_eth_handle_t eth_handle;
	struct w5100_config_t w5100_cfg;
	char *hostname;
} *eth_cfgs;

static void eth_enable_static_ip( const struct eth_static_ip *const sip )
{
	esp_netif_ip_info_t old_ip_info;

	ESP_ERROR_CHECK( esp_netif_get_ip_info( eth_cfgs->eth_netif, &old_ip_info ) );
	ESP_ERROR_CHECK( esp_netif_dhcpc_stop( eth_cfgs->eth_netif ) );
	ESP_ERROR_CHECK( esp_netif_set_old_ip_info( eth_cfgs->eth_netif, &old_ip_info ) );
	ESP_ERROR_CHECK( esp_netif_set_ip_info( eth_cfgs->eth_netif, &sip->net ) );

	if ( !ip4_addr_isany_val( sip->p_dns ) )
		ESP_ERROR_CHECK( esp_netif_set_dns_info(
			eth_cfgs->eth_netif,
			ESP_NETIF_DNS_MAIN,
			&( esp_netif_dns_info_t ) { .ip.u_addr.ip4 = sip->p_dns } ) );
	if ( !ip4_addr_isany_val( sip->s_dns ) )
		ESP_ERROR_CHECK( esp_netif_set_dns_info(
			eth_cfgs->eth_netif,
			ESP_NETIF_DNS_BACKUP,
			&( esp_netif_dns_info_t ) { .ip.u_addr.ip4 = sip->s_dns } ) );
	if ( !ip4_addr_isany_val( sip->f_dns ) )
		ESP_ERROR_CHECK( esp_netif_set_dns_info(
			eth_cfgs->eth_netif,
			ESP_NETIF_DNS_FALLBACK,
			&( esp_netif_dns_info_t ) { .ip.u_addr.ip4 = sip->f_dns } ) );

#if CONFIG_LOG_MAXIMUM_LEVEL >= 3
	esp_netif_ip_info_t new_ip_info;
	esp_netif_dns_info_t p_dns, s_dns, f_dns;

	ESP_ERROR_CHECK( esp_netif_get_ip_info( eth_cfgs->eth_netif, &new_ip_info ) );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_MAIN, &p_dns ) );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_BACKUP, &s_dns ) );
	ESP_ERROR_CHECK( esp_netif_get_dns_info( eth_cfgs->eth_netif, ESP_NETIF_DNS_FALLBACK, &f_dns ) );

	// clang-format off
	ESP_LOGI( TAG,	"Static IPv4 defined as (DNS only if valid):\n"
					"\tIP: " IPSTR "\n"
					"\tMASK: " IPSTR "\n"
					"\tGW: " IPSTR "\n"
					"\tP_DNS: " IPSTR "\n"
					"\tS_DNS: " IPSTR "\n"
					"\tF_DNS: " IPSTR "\n",
					IP2STR( &new_ip_info.ip ),
					IP2STR( &new_ip_info.netmask ),
					IP2STR( &new_ip_info.gw ),
					IP2STR( &p_dns.ip.u_addr.ip4 ),
					IP2STR( &s_dns.ip.u_addr.ip4 ),
					IP2STR( &f_dns.ip.u_addr.ip4 ) );
	// clang-format on
#endif
}

static void eth_event_handler_hostname( void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data )
{
	ESP_ERROR_CHECK( esp_netif_set_hostname( eth_cfgs->eth_netif, eth_cfgs->hostname ) );
}

#ifdef CONFIG_TEST_DEINIT
void eth_deinit( void )
{
	ESP_ERROR_CHECK( esp_eth_stop( eth_cfgs->eth_handle ) );
	// ESP_ERROR_CHECK( esp_eth_del_netif_glue( eth_cfgs->eth_netif_glue ) );
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL( 4, 4, 0 )
	ESP_ERROR_CHECK( esp_eth_clear_default_handlers( eth_cfgs->eth_netif ) );
#endif
	ESP_ERROR_CHECK( esp_eth_driver_uninstall( eth_cfgs->eth_handle ) );
	ESP_ERROR_CHECK( eth_cfgs->eth_config.phy->del( eth_cfgs->eth_config.phy ) );
	ESP_ERROR_CHECK( eth_cfgs->eth_config.mac->del( eth_cfgs->eth_config.mac ) );
	esp_netif_destroy( eth_cfgs->eth_netif );
	eth_cfgs->w5100_cfg.deinit();
	free( eth_cfgs );
}
#endif

void eth_init( const struct eth_ifconfig *const cfg )
{
	ESP_ERROR_CHECK( !( eth_cfgs = calloc( 1, sizeof *eth_cfgs ) ) );
	ESP_ERROR_CHECK( esp_event_handler_instance_register(
		ETH_EVENT,
		ETHERNET_EVENT_START,
		&eth_event_handler_hostname,
		NULL,
		NULL ) );

	eth_cfgs->eth_netif = esp_netif_new( &( esp_netif_config_t ) {
		.base = &( esp_netif_inherent_config_t )ESP_NETIF_INHERENT_DEFAULT_ETH(),
		.stack = ESP_NETIF_NETSTACK_DEFAULT_ETH } );

	if ( cfg )
	{
		if ( *cfg->hostname )
			eth_cfgs->hostname = strdup( cfg->hostname );

		if ( cfg->sip.net.ip.addr )
			eth_enable_static_ip( &cfg->sip );
	}

	memcpy( &eth_cfgs->w5100_cfg, &cfg->w5100_cfg, sizeof cfg->w5100_cfg );
	pre_init_setup( &eth_cfgs->w5100_cfg );

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL( 4, 4, 0 )
	// Set default handlers to process TCP/IP stuffs
	ESP_ERROR_CHECK( esp_eth_set_default_handlers( eth_cfgs->eth_netif ) );
#endif

	esp_eth_mac_t *mac = esp_eth_mac_new_w5100( &( const eth_mac_config_t )ETH_MAC_DEFAULT_CONFIG() );
	esp_eth_phy_t *phy = esp_eth_phy_new_w5100( NULL );	 // No PHY pins connected

	esp_eth_config_t eth_cfg = ETH_DEFAULT_CONFIG( mac, phy );
	eth_cfgs->eth_config = eth_cfg;

	eth_cfgs->w5100_cfg.init();

	ESP_ERROR_CHECK( esp_eth_driver_install( &eth_cfgs->eth_config, &eth_cfgs->eth_handle ) );

	uint8_t mac_addr[ 6 ];
	ESP_ERROR_CHECK( esp_read_mac( mac_addr, ESP_MAC_ETH ) );
	ESP_ERROR_CHECK( esp_eth_ioctl( eth_cfgs->eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr ) );

	/* attach Ethernet driver to TCP/IP stack */
	ESP_ERROR_CHECK( esp_netif_attach( eth_cfgs->eth_netif, esp_eth_new_netif_glue( eth_cfgs->eth_handle ) ) );
	/* start Ethernet driver state machine */
	ESP_ERROR_CHECK( esp_eth_start( eth_cfgs->eth_handle ) );
}
