
#pragma once

#include "esp_eth_mac.h"
#include "esp_eth_phy.h"

esp_eth_mac_t *esp_eth_mac_new_w5100( const eth_mac_config_t *const mac_config );

esp_eth_phy_t *esp_eth_phy_new_w5100( const eth_phy_config_t *const phy_config );