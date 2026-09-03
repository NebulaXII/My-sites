# Geyser Diverter Firmware — V0.1

First real stage of the ESP32-S3 firmware, per `../SPECIFICATION.md` §28's build order:

> **V0.1** — ESP32 boot + LEDs + watchdog.

That's all this is. No Wi-Fi, no sensors, no power output — those are later stages
(V0.2 onward). This stage exists to prove the two things everything else depends on:
the board boots into a known-safe state, and the watchdog actually recovers it if
the firmware ever hangs.

**Do not wire this to the 230V power stage.** There's nothing in this firmware that
drives a relay, SSR, or contactor yet — there's nothing *to* connect. Per the
specification's own closing note, the power stage doesn't get touched until the
low-voltage firmware has been tested stage by stage.

## What it does

- Boots, sets every output pin to a safe LOW default before anything else runs.
- Arms the hardware watchdog (5s timeout) and feeds it every loop iteration.
- Blinks `LED_HEARTBEAT` (GPIO7) at 1Hz and logs the same heartbeat over serial —
  so you can confirm it's alive even before any LEDs are wired up.
- Has a minimal fault flag (`faults.h/.cpp`) driving `LED_FAULT` (GPIO8), currently
  only used by the watchdog self-test below. Real fault sources plug into this in
  later stages (Specification §16).
- `LED_LINK` (GPIO9) is initialized to a safe off state but otherwise unused until
  Wi-Fi lands in V0.2.

Pin numbers come from `../PIN_ASSIGNMENT.md` §8 via `src/pins.h` — that file is the
single place to change if your board's wiring differs.

## Build and flash

Requires [PlatformIO](https://platformio.org/) (`pip install platformio`, or the
VS Code extension).

```
pio run                 # build
pio run -t upload       # flash (board connected over USB)
pio device monitor       # serial log at 115200 baud
```

`platformio.ini` targets a generic ESP32-S3 devkit (`esp32-s3-devkitc-1`). If
PlatformIO doesn't recognize your specific board, that board id still works for
almost any ESP32-S3-WROOM-1-based devkit — swap it only if you hit a real
mismatch (wrong flash size, PSRAM variant, etc).

## What you should see

**No LEDs wired yet?** Just open the serial monitor — every heartbeat line there
is the same signal the LED shows:

```
[BOOT] Geyser Diverter firmware V0.1 (system/safety/faults only)
[BOOT] Outputs initialized to safe (LOW) default state
[BOOT] Watchdog armed: 5s timeout
[HEARTBEAT] alive, uptime=0s, fault=no
[HEARTBEAT] alive, uptime=1s, fault=no
...
```

**LEDs wired to GPIO7/8/9?** `LED_HEARTBEAT` should blink steadily once per second.
`LED_FAULT` and `LED_LINK` stay off during normal operation.

## Testing the watchdog

This is the one behavior in V0.1 worth actually proving on the bench, not just
trusting the code:

1. Hold the board's **BOOT** button (GPIO0) while it powers up or resets.
2. It logs that the watchdog test is armed, runs normally for 5 seconds, then
   deliberately hangs forever (`while(true){}`) — the same failure mode as a
   firmware bug that stops responding.
3. Within ~5 seconds the watchdog should reset the board on its own. You'll see
   the boot log print again with no button held, and normal heartbeats resume.

If it doesn't reset — if the board just sits there hung — that's the one thing in
this stage that would actually matter for safety later (Specification §17: on an
unresponsive firmware, output must default off and the system must come back up
safe), so don't move on to V0.2 until this reliably recovers.

## Not built yet

Everything else in the staged plan (Specification §28): Wi-Fi + local web UI
(V0.2), PT1000 temperature (V0.3), CT current sensing (V0.4), manual SSR control
(V0.5), inverter RS-485 (V0.6), the surplus-solar algorithm (V0.7), battery
protection (V0.8), automatic control (V0.9), cloud pairing (V0.10), then full
fault handling + data logging (V1.0). Each stage should get the same treatment
this one did: build it, bench-test it on real hardware, confirm it fails safe,
*then* move to the next stage — never all at once, and never near mains power
until the stage that actually needs it says so.
