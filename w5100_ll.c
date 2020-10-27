
#include "sdkconfig.h"

#if CONFIG_W5100_SPI_LOCK
#	include "freertos/FreeRTOS.h"
#	include "freertos/task.h"
#	include "freertos/semphr.h"
#endif
#if CONFIG_W5100_SPI_EN_MANUAL
#	include "driver/gpio.h"
#endif
#include "esp_attr.h"
#include "driver/spi_master.h"

#include "w5100_ll.h"

#if CONFIG_W5100_POLLING_SPI_TRANS
#	define W5100_TR spi_device_polling_transmit
#else
#	define W5100_TR spi_device_transmit
#endif

spi_device_handle_t w5100_spi_handle;
#if CONFIG_W5100_SPI_LOCK
SemaphoreHandle_t eth_mutex;
bool user_provided_mutex;
#endif

DMA_ATTR uint32_t tx_tr, rx_tr;

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
	ESP_ERROR_CHECK( gpio_set_direction( CONFIG_W5100_RESET_GPIO, GPIO_MODE_OUTPUT ) );
	ESP_ERROR_CHECK( gpio_set_level( CONFIG_W5100_RESET_GPIO, 0 ) );
	vTaskDelay( 1 );
	ESP_ERROR_CHECK( gpio_set_level( CONFIG_W5100_RESET_GPIO, 1 ) );
}

void w5100_spi_mtx_set( void *spi_mtx )
{
#if CONFIG_W5100_SPI_LOCK
	ESP_ERROR_CHECK( !spi_mtx );
	eth_mutex = ( SemaphoreHandle_t )spi_mtx;
	user_provided_mutex = true;
#endif
}

void w5100_spi_init( void )
{
#if CONFIG_W5100_SPI_EN_MANUAL
	ESP_ERROR_CHECK( gpio_set_direction( CONFIG_W5100_SPI_EN_GPIO, GPIO_MODE_OUTPUT ) );
#endif
#if CONFIG_W5100_SPI_LOCK
	if ( user_provided_mutex )
		xSemaphoreTake( eth_mutex, portMAX_DELAY );
	else
		ESP_ERROR_CHECK( !( eth_mutex = xSemaphoreCreateMutex() ) );
#endif
	ESP_ERROR_CHECK( spi_bus_add_device(
		CONFIG_W5100_SPI_BUS - 1,
		&( spi_device_interface_config_t ) {
			.clock_speed_hz = CONFIG_W5100_SPI_CLCK, .spics_io_num = CONFIG_W5100_CS, .queue_size = 1,
#if CONFIG_W5100_SPI_EN_MANUAL
			.pre_cb = w5100_SPI_EN_assert, .post_cb = w5100_SPI_En_deassert
#endif
		},
		&w5100_spi_handle ) );
#if CONFIG_W5100_SPI_LOCK
	if ( user_provided_mutex )
		xSemaphoreGive( eth_mutex );
#endif
#if CONFIG_W5100_SPI_BUS_ACQUIRE
	ESP_ERROR_CHECK( spi_device_acquire_bus( w5100_spi_handle, portMAX_DELAY ) );
#endif
}

void w5100_spi_deinit( void )
{
#if CONFIG_W5100_SPI_LOCK
	if ( user_provided_mutex )
		xSemaphoreTake( eth_mutex, portMAX_DELAY );
#endif
#if CONFIG_W5100_SPI_BUS_ACQUIRE
	spi_device_release_bus( w5100_spi_handle );
#endif
	ESP_ERROR_CHECK( spi_bus_remove_device( w5100_spi_handle ) );
#if CONFIG_W5100_SPI_LOCK
	if ( user_provided_mutex )
		xSemaphoreGive( eth_mutex );
	else
		vSemaphoreDelete( eth_mutex );
#endif
}

void w5100_spi_op( uint32_t tx, uint32_t *rx )
{
#if CONFIG_W5100_SPI_LOCK
	xSemaphoreTake( eth_mutex, portMAX_DELAY );
#endif
	tx_tr = tx, rx_tr = rx ? *rx : ( uint32_t )NULL;
	ESP_ERROR_CHECK( W5100_TR( w5100_spi_handle,
		&( spi_transaction_t ){ .length = 32, .tx_buffer = &tx_tr, .rx_buffer = &rx_tr } ) );
	if ( rx )
		*rx = rx_tr;
#if CONFIG_W5100_SPI_LOCK
	xSemaphoreGive( eth_mutex );
#endif
}