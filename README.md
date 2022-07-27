## Port of the WIZnet W5100 chip to ESP32's esp-idf.

This is incomplete, and some initialization
boilerplate and lower-level layer of the driver
must be provided. The ESP32_W5100 superproject is
provided as a baseline.

Some polling operations are infinite loops which
I haven't ever had any problems with, but the
watchdog should be enabled just in case, or the
implementation changed.

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
Public Domain with a plea to whoever
clones/forks/etc to contribute back any
improvements.
