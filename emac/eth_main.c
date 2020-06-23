/* w5100 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "eth_main.h"
#include "sdkconfig.h"

#include "w5100_spi.h"

static const char *TAG = "eth_example";

void eth_main( void )
{
	esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
	esp_netif_t *eth_netif = esp_netif_new( &netif_cfg );
	// Set default handlers to process TCP/IP stuffs
	ESP_ERROR_CHECK( esp_eth_set_default_handlers( eth_netif ) );

	w5100_spi_init();

	eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
	mac_config.smi_mdc_gpio_num = -1; // w5100 doesn't have SMI interface
	mac_config.smi_mdio_gpio_num = -1;
	esp_eth_mac_t *mac = esp_eth_mac_new_w5100( &mac_config );

	eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
	phy_config.autonego_timeout_ms = 0; // w5100 doesn't support auto-negotiation
	phy_config.reset_gpio_num = -1;		// w5100 doesn't have a pin to reset internal PHY
	esp_eth_phy_t *phy = esp_eth_phy_new_w5100( &phy_config );

	esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG( mac, phy );
	esp_eth_handle_t eth_handle = NULL;
	ESP_ERROR_CHECK( esp_eth_driver_install( &eth_config, &eth_handle ) );

	/* w5100 doesn't burn any factory MAC address, we need to set it manually.
	   02:00:00 is a Locally Administered OUI range so should not be used except when testing on a LAN under your
	   control.
	*/

	/* attach Ethernet driver to TCP/IP stack */
	ESP_ERROR_CHECK( esp_netif_attach( eth_netif, esp_eth_new_netif_glue( eth_handle ) ) );
	/* start Ethernet driver state machine */
	ESP_ERROR_CHECK( esp_eth_start( eth_handle ) );
}
