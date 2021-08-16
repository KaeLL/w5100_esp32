## Port of the WIZnet W5100 chip to ESP32's esp-idf.

This is incomplete, and some initialization
boilerplate and lower-level layer of the driver
must be provided. The ESP32_W5100 superproject is
provided as a baseline.

Roadmap:
- Done:
    - Static IP
    - De-initialization routines

- Currently being worked:
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
