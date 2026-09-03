#pragma once

// Minimal in-RAM fault flag for V0.1 bring-up only.
//
// Real fault codes, latching-with-required-acknowledgement, and persistence
// across a reset (Specification §16) are a later stage's job — this module
// only proves the LED + logging path end to end so later stages have
// something to plug real fault sources into.

namespace Faults {
  void begin();
  void raise(const char *reason);
  void clear();
  bool active();
}
