Port of the WIZnet W5100 chip to ESP32's esp-idf.

The repo structure is modeled after the standard
esp32 app component, making it plug n' play,
with the exception that particular details about
your board design have to be changed on menuconfig.

This is incomplete, and some initialization
boilerplate must be provided.

By default, my board design is assumed. This is
reflected on GPIO's used, manual control of the
SPI_EN pin, and the use of polling instead of
interrupts.