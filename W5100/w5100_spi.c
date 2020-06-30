
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "w5100_spi.h"

#define W5100_SPI_CLK_HZ 300000
#define W5100_CS		 GPIO_NUM_17

spi_device_handle_t w5100_spi_handle;
SemaphoreHandle_t eth_mutex;

void IRAM_ATTR w5100_SPI_EN_assert( spi_transaction_t *trans )
{
	gpio_set_level( GPIO_NUM_22, 1 );
}

void IRAM_ATTR w5100_SPI_En_deassert( spi_transaction_t *trans )
{
	gpio_set_level( GPIO_NUM_22, 0 );
}

void w5100_spi_init( void )
{
	gpio_set_direction( GPIO_NUM_22, GPIO_MODE_OUTPUT );
	ESP_ERROR_CHECK( spi_bus_add_device( SPI3_HOST,
		&( spi_device_interface_config_t ){ .clock_speed_hz = W5100_SPI_CLK_HZ,
			.spics_io_num = W5100_CS,
			.queue_size = 20,
			.pre_cb = w5100_SPI_EN_assert,
			.post_cb = w5100_SPI_En_deassert },
		&w5100_spi_handle ) );

	ESP_ERROR_CHECK( !( eth_mutex = xSemaphoreCreateMutex() ) );

	// spi_device_acquire_bus( w5100_spi_handle, portMAX_DELAY );
}

void spi_transaction( uint32_t tx, uint32_t *rx )
{
	xSemaphoreTake( eth_mutex, portMAX_DELAY );
	ESP_ERROR_CHECK( spi_device_transmit( w5100_spi_handle,
		&( spi_transaction_t ){ .length = 32, .tx_buffer = &tx, .rx_buffer = rx } ) );
	xSemaphoreGive( eth_mutex );
}