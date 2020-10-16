## Port of the WIZnet W5100 chip to ESP32's esp-idf.

The repo structure is modeled after the standard
esp32 app component, making it plug n' play for
your project, with the exception that particular
details about your board design have to be changed
on menuconfig or even ported/developed from scratch.

This is incomplete, and some initialization
boilerplate must be provided. The ESP32_W5100
superproject is provided as a baseline.

By default, my board design is assumed. This is
reflected on GPIO's used, manual control of the
SPI_EN pin, and the use of polling instead
of interrupts for Slave -> Master communication.

Roadmap:
- Done:
    - Static IP

- Currently being worked:
    - De-initialization routines
    - Stability improvements, especially SPI-related

- Future:
    - Tests
    - Documentation
    - Refactoring and more stability improvements
    - ...

## My license
Public Domain with a plead to whoever
forks/clones/etc to contribute improvements back,
by at least opening an issue with a suggestion and
at most submiting a PR.
