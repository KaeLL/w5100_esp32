
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_eth_mac.h"
#include "esp_eth_phy.h"

// #define LOG_FUN(X) ESP_LOGW(TAG, X " %s:%d", __func__, __LINE__)
#define LOG_FUN( X )

#define START LOG_FUN( "start" )
#define END	  LOG_FUN( "end" );

/**
 * @brief Create w5100 Ethernet MAC instance
 *
 * @param[in] enc28j60_config: w5100 specific configuration
 * @param[in] mac_config: Ethernet MAC configuration
 *
 * @return
 *      - instance: create MAC instance successfully
 *      - NULL: create MAC instance failed because some error occurred
 */
esp_eth_mac_t *esp_eth_mac_new_w5100( const eth_mac_config_t *mac_config );

/**
 * @brief Create a PHY instance of w5100
 *
 * @param[in] config: configuration of PHY
 *
 * @return
 *      - instance: create PHY instance successfully
 *      - NULL: create PHY instance failed because some error occurred
 */
esp_eth_phy_t *esp_eth_phy_new_w5100( const eth_phy_config_t *config );

void eth_main( void );

#ifdef __cplusplus
}
#endif
