
#include "sdkconfig.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "w5100_debug.h"
#include "w5100.h"
#include "w5100_ll.h"
#include "eth_main.h"

tag_def( "w5100_eth_phy" );
#define PHY_CHECK( a, str, goto_tag, ... )                                          \
	do                                                                              \
	{                                                                               \
		if ( !( a ) )                                                               \
		{                                                                           \
			ESP_LOGE( TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__ ); \
			goto goto_tag;                                                          \
		}                                                                           \
	} while ( 0 )

typedef struct
{
	esp_eth_phy_t parent;
	esp_eth_mediator_t *eth;
} phy_w5100_t;

static esp_err_t w5100_set_mediator( esp_eth_phy_t *phy, esp_eth_mediator_t *eth )
{
	f_entry();
	PHY_CHECK( eth, "can't set mediator for w5100 to null", err );
	phy_w5100_t *w5100 = __containerof( phy, phy_w5100_t, parent );
	w5100->eth = eth;
	f_exit();

	return ESP_OK;
err:
	f_exit();
	return ESP_ERR_INVALID_ARG;
}

static esp_err_t w5100_get_link( esp_eth_phy_t *phy )
{
	f_entry();
	phy_w5100_t *w5100 = __containerof( phy, phy_w5100_t, parent );

	static bool been_here;

	if ( !been_here )
	{
		w5100->eth->on_state_changed( w5100->eth, ETH_STATE_SPEED, ( void * )ETH_SPEED_100M );
		w5100->eth->on_state_changed( w5100->eth, ETH_STATE_DUPLEX, ( void * )ETH_DUPLEX_FULL );
		w5100->eth->on_state_changed( w5100->eth, ETH_STATE_LINK, ( void * )ETH_LINK_UP );
		been_here = true;
	}
	f_exit();

	return ESP_OK;
}

static esp_err_t w5100_reset( esp_eth_phy_t *phy )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_reset_hw( esp_eth_phy_t *phy )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_negotiate( esp_eth_phy_t *phy )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_pwrctl( esp_eth_phy_t *phy, bool enable )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_set_addr( esp_eth_phy_t *phy, uint32_t addr )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_get_addr( esp_eth_phy_t *phy, uint32_t *addr )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_del( esp_eth_phy_t *phy )
{
	f_entry();
	phy_w5100_t *w5100 = __containerof( phy, phy_w5100_t, parent );
	free( w5100 );
	f_exit();

	return ESP_OK;
}

static esp_err_t w5100_init( esp_eth_phy_t *phy )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

static esp_err_t w5100_deinit( esp_eth_phy_t *phy )
{
	f_entry();
	f_exit();
	return ESP_OK;
}

esp_eth_phy_t *esp_eth_phy_new_w5100( const eth_phy_config_t *config )
{
	f_entry();
	phy_w5100_t *w5100 = calloc( 1, sizeof( phy_w5100_t ) );
	PHY_CHECK( w5100, "calloc w5100 failed", err );
	w5100->parent.set_mediator = w5100_set_mediator;
	w5100->parent.reset = w5100_reset;
	w5100->parent.reset_hw = w5100_reset_hw;
	w5100->parent.init = w5100_init;
	w5100->parent.deinit = w5100_deinit;
	w5100->parent.negotiate = w5100_negotiate;
	w5100->parent.get_link = w5100_get_link;
	w5100->parent.pwrctl = w5100_pwrctl;
	w5100->parent.set_addr = w5100_set_addr;
	w5100->parent.get_addr = w5100_get_addr;
	w5100->parent.del = w5100_del;
	f_exit();

	return &( w5100->parent );
err:
	f_exit();
	return NULL;
}
