#pragma once

// Single source of truth for GPIO assignments — see ../../PIN_ASSIGNMENT.md.
// Every other pin in that document beyond what's listed here is reserved for
// a later stage and isn't touched by this firmware yet.

namespace Pins {
  constexpr int LED_HEARTBEAT = 7;  // §8: slow blink = firmware alive
  constexpr int LED_FAULT     = 8;  // §8: on = fault latched
  constexpr int LED_LINK      = 9;  // §8: Wi-Fi status, driven from V0.2
  constexpr int WATCHDOG_TEST_PIN = 0;  // BOOT button — hold at power-on to test the watchdog reset path

  // §3: PT1000 via MAX31865 (SPI), added V0.3
  constexpr int TEMP_SPI_MOSI = 11;
  constexpr int TEMP_SPI_SCLK = 12;
  constexpr int TEMP_SPI_MISO = 13;
  constexpr int TEMP_SPI_CS   = 14;
  constexpr int TEMP_DRDY     = 15;  // not used yet — this stage polls instead of using the interrupt
}
