
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#if CONFIG_W5100_SPI_LOCK
#include "freertos/semphr.h"
#endif
#include "freertos/task.h"
#if CONFIG_W5100_SPI_EN_MANUAL
#include "driver/gpio.h"
#endif
#include "esp_attr.h"
#include "driver/spi_master.h"

#include "w5100_ll.h"

typedef struct
{
	spi_device_handle_t w5100_spi_handle;
	SemaphoreHandle_t eth_mutex;
	bool user_provided_mutex;
} w5100_ll_t;

w5100_ll_t *w5100_ll_data;
DMA_ATTR uint32_t tx_tr, rx_tr;

#if CONFIG_W5100_POLLING_SPI_TRANS
#define W5100_TR spi_device_polling_transmit
#else
#define W5100_TR spi_device_transmit
#endif

#if CONFIG_W5100_SPI_EN_MANUAL
void IRAM_ATTR w5100_SPI_EN_assert( spi_transaction_t *trans )
{
	GPIO.out_w1ts = ( 1 << CONFIG_W5100_SPI_EN_GPIO );
}

void IRAM_ATTR w5100_SPI_En_deassert( spi_transaction_t *trans )
{
	GPIO.out_w1tc = ( 1 << CONFIG_W5100_SPI_EN_GPIO );
}
#endif

void w5100_ll_hw_reset( void )
{
	// /RESET is inverted in my board
	ESP_ERROR_CHECK( gpio_set_level( CONFIG_W5100_RESET_GPIO, 1 ) );
	vTaskDelay( 1 );
	ESP_ERROR_CHECK( gpio_set_level( CONFIG_W5100_RESET_GPIO, 0 ) );
}

void w5100_spi_init( const struct w5100_config_t *const w5100_cfg )
{
	ESP_ERROR_CHECK( gpio_config( &( const gpio_config_t ) {
		BIT64( CONFIG_W5100_RESET_GPIO ),
		GPIO_MODE_OUTPUT,
		GPIO_PULLUP_DISABLE,
		GPIO_PULLDOWN_DISABLE,
		GPIO_INTR_DISABLE } ) );
#if CONFIG_W5100_SPI_EN_MANUAL
	ESP_ERROR_CHECK( gpio_config( &( const gpio_config_t ) {
		BIT64( CONFIG_W5100_SPI_EN_GPIO ),
		GPIO_MODE_OUTPUT,
		GPIO_PULLUP_DISABLE,
		GPIO_PULLDOWN_DISABLE,
		GPIO_INTR_DISABLE } ) );
#endif
	w5100_ll_data = calloc( 1, sizeof( w5100_ll_t ) );
	ESP_ERROR_CHECK( !w5100_ll_data );
#if CONFIG_W5100_SPI_LOCK
	if ( w5100_cfg && w5100_cfg->user_mutex )
		w5100_ll_data->eth_mutex = ( SemaphoreHandle_t )w5100_cfg->user_mutex,
		w5100_ll_data->user_provided_mutex = true;
	else
		ESP_ERROR_CHECK( !( w5100_ll_data->eth_mutex = xSemaphoreCreateMutex() ) );

	ESP_ERROR_CHECK( pdFALSE == xSemaphoreTake( w5100_ll_data->eth_mutex, portMAX_DELAY ) );
#endif
	ESP_ERROR_CHECK( spi_bus_add_device(
		CONFIG_W5100_SPI_BUS - 1,
		&( spi_device_interface_config_t )
		{
			.clock_speed_hz = CONFIG_W5100_SPI_CLCK, .spics_io_num = CONFIG_W5100_CS, .queue_size = 1,
#if CONFIG_W5100_SPI_EN_MANUAL
			.pre_cb = w5100_SPI_EN_assert, .post_cb = w5100_SPI_En_deassert
#endif
		},
		&w5100_ll_data->w5100_spi_handle ) );
#if CONFIG_W5100_SPI_BUS_ACQUIRE
	ESP_ERROR_CHECK( spi_device_acquire_bus( w5100_ll_data->w5100_spi_handle, portMAX_DELAY ) );
#endif
#if CONFIG_W5100_SPI_LOCK
	ESP_ERROR_CHECK( pdFALSE == xSemaphoreGive( w5100_ll_data->eth_mutex ) );
#endif
}

void w5100_spi_deinit( void )
{
#if CONFIG_W5100_SPI_LOCK
	ESP_ERROR_CHECK( pdFALSE == xSemaphoreTake( w5100_ll_data->eth_mutex, portMAX_DELAY ) );
#endif
#if CONFIG_W5100_SPI_BUS_ACQUIRE
	spi_device_release_bus( w5100_ll_data->w5100_spi_handle );
#endif
	ESP_ERROR_CHECK( spi_bus_remove_device( w5100_ll_data->w5100_spi_handle ) );
#if CONFIG_W5100_SPI_LOCK
	xSemaphoreHandle tmp_lock = w5100_ll_data->eth_mutex;
	bool usr_prov_mtx = w5100_ll_data->user_provided_mutex;

	free( w5100_ll_data );
	w5100_ll_data = NULL;
	ESP_ERROR_CHECK( pdFALSE == xSemaphoreGive( tmp_lock ) );

	if ( !usr_prov_mtx )
		vSemaphoreDelete( tmp_lock );
#endif
}

void w5100_spi_op( const uint32_t tx, uint32_t *const rx )
{
	if ( !w5100_ll_data )
		return;
#if CONFIG_W5100_SPI_LOCK
	ESP_ERROR_CHECK( pdFALSE == xSemaphoreTake( w5100_ll_data->eth_mutex, portMAX_DELAY ) );
#endif
	tx_tr = tx, rx_tr = rx ? *rx : ( uint32_t )NULL;
	ESP_ERROR_CHECK(
		W5100_TR( w5100_ll_data->w5100_spi_handle,
				  &( spi_transaction_t ) { .length = 32, .tx_buffer = &tx_tr, .rx_buffer = &rx_tr } ) );
	if ( rx )
		*rx = rx_tr;
#if CONFIG_W5100_SPI_LOCK
	ESP_ERROR_CHECK( pdFALSE == xSemaphoreGive( w5100_ll_data->eth_mutex ) );
#endif
}