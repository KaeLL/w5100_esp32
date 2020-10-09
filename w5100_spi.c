
#include "sdkconfig.h"

#if CONFIG_W5100_SPI_LOCK
#	include "freertos/FreeRTOS.h"
#	include "freertos/semphr.h"
#endif
#if CONFIG_W5100_SPI_EN_MANUAL
#	include "driver/gpio.h"
#endif
#if !CONFIG_W5100_CUSTOM_SPI_TRANS
#	include "esp_attr.h"
#endif
#include "driver/spi_master.h"

#include "w5100_spi.h"

spi_device_handle_t w5100_spi_handle;
#if CONFIG_W5100_SPI_LOCK
SemaphoreHandle_t eth_mutex;
#endif

#if !CONFIG_W5100_CUSTOM_SPI_TRANS
DMA_ATTR uint32_t tx_tr, rx_tr;
#endif

#if CONFIG_W5100_CUSTOM_SPI_TRANS
#	define W5100_TR spi_trans_cb

void ( *spi_trans_cb )( spi_device_handle_t spi, uint32_t buf_w, uint32_t *buf_r );

void set_spi_trans_cb( spi_cb_t spi_cb )
{
	spi_trans_cb = spi_cb;
}
#elif CONFIG_W5100_POLLING_SPI_TRANS
#	define W5100_TR spi_device_polling_transmit
#else
#	define W5100_TR spi_device_transmit
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

void w5100_spi_init( void )
{
#if CONFIG_W5100_SPI_EN_MANUAL
	gpio_set_direction( CONFIG_W5100_SPI_EN_GPIO, GPIO_MODE_OUTPUT );
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
	ESP_ERROR_CHECK( !( eth_mutex = xSemaphoreCreateMutex() ) );
#endif

#if CONFIG_W5100_SPI_BUS_ACQUIRE
	ESP_ERROR_CHECK( spi_device_acquire_bus( w5100_spi_handle, portMAX_DELAY ) );
#endif
}

void w5100_spi_deinit( void )
{
#if CONFIG_W5100_SPI_BUS_ACQUIRE
	spi_device_release_bus( w5100_spi_handle );
#endif
	ESP_ERROR_CHECK( spi_bus_remove_device( w5100_spi_handle ) );
#if CONFIG_W5100_SPI_LOCK
	vSemaphoreDelete( eth_mutex );
#endif
}

void w5100_spi_op( uint32_t tx, uint32_t *rx )
{
#if CONFIG_W5100_SPI_LOCK
	xSemaphoreTake( eth_mutex, portMAX_DELAY );
#endif
#if CONFIG_W5100_CUSTOM_SPI_TRANS
	W5100_TR( w5100_spi_handle, tx, rx );
#else
	tx_tr = tx, rx_tr = rx ? *rx : ( uint32_t )NULL;
	ESP_ERROR_CHECK( W5100_TR( w5100_spi_handle,
		&( spi_transaction_t ){ .length = 32, .tx_buffer = &tx_tr, .rx_buffer = &rx_tr } ) );
	if ( rx )
		*rx = rx_tr;
#endif
#if CONFIG_W5100_SPI_LOCK
	xSemaphoreGive( eth_mutex );
#endif
}