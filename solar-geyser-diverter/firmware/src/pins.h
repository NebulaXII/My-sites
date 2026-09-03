#pragma once

// Single source of truth for GPIO assignments — see ../../PIN_ASSIGNMENT.md.
// V0.1 only drives the three status LEDs; every other pin in that document
// is reserved for later stages and isn't touched by this firmware yet.

namespace Pins {
  constexpr int LED_HEARTBEAT = 7;  // §8: slow blink = firmware alive
  constexpr int LED_FAULT     = 8;  // §8: on = fault latched
  constexpr int LED_LINK      = 9;  // §8: reserved for Wi-Fi status from V0.2 onward
  constexpr int WATCHDOG_TEST_PIN = 0;  // BOOT button — hold at power-on to test the watchdog reset path
}
