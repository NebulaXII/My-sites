# Geyser Diverter Firmware — V0.2

Staged ESP32-S3 firmware per `../SPECIFICATION.md` §28's build order. Currently
implements:

> **V0.1** — ESP32 boot + LEDs + watchdog.
> **V0.2** — Wi-Fi + web interface.

No sensors, no power output, nothing that touches the 230V stage — that's still
several stages out. Per the specification's own closing note, the power stage
doesn't get wired up until each low-voltage stage before it has been bench-tested.

**Do not wire this to the 230V power stage.** There is nothing in this firmware
that drives a relay, SSR, or contactor — there's nothing to connect yet.

## What it does

**V0.1 (system/safety):**
- Boots, sets every output pin to a safe LOW default before anything else runs.
- Arms the hardware watchdog (5s timeout) and feeds it every loop iteration.
- Blinks `LED_HEARTBEAT` (GPIO7) at 1Hz and logs the same heartbeat over serial.
- Minimal fault flag (`faults.h/.cpp`) driving `LED_FAULT` (GPIO8) — real fault
  sources plug into this in later stages (Specification §16).

**V0.2 (Wi-Fi + local web UI), added this stage:**
- `settings.h/.cpp` — Wi-Fi credentials and device name persisted in NVS
  (`Preferences`), so they survive a reboot or power loss (Specification §11).
- `wifi_manager.h/.cpp` — a non-blocking connection state machine: tries the
  saved network on boot, falls back to its own setup Wi-Fi network (SoftAP) if
  there are no saved credentials or the connection times out (15s). Drives
  `LED_LINK` (GPIO9): fast blink = setup mode, slow blink = connecting, solid
  on = connected.
- `web_ui.h/.cpp` — a local web server (port 80) with:
  - `/` — status: device name, Wi-Fi state, IP, uptime, fault state. Sensor
    rows are explicitly marked "not yet installed" rather than faking numbers.
  - `/wifi` — form to set the Wi-Fi network, password, and device name.
  - `/wifi/reset` — forget the saved network and restart into setup mode.
  - `/api/status` — the same status as JSON, for future use.

Pin numbers come from `../PIN_ASSIGNMENT.md` §8 via `src/pins.h` — the single
place to change if your board's wiring differs.

## Build and flash

Requires [PlatformIO](https://platformio.org/) (`pip install platformio`, or the
VS Code extension).

```
pio run                  # build
pio run -t upload        # flash (board connected over USB)
pio device monitor        # serial log at 115200 baud
```

`platformio.ini` targets a generic ESP32-S3 devkit (`esp32-s3-devkitc-1`). If
PlatformIO doesn't recognize your specific board, that board id still works for
almost any ESP32-S3-WROOM-1-based devkit — swap it only if you hit a real
mismatch (wrong flash size, PSRAM variant, etc).

## First boot — connecting it to your Wi-Fi

1. Flash and power up. With no saved credentials yet, it starts its own setup
   network: watch the serial log for `AP mode: SSID 'GeyserDiverter-XXXX'`.
2. On your phone/laptop, join that Wi-Fi network.
3. Browse to **http://192.168.4.1/** → **Wi-Fi & device settings**.
4. Enter your home Wi-Fi SSID/password and a device name, submit.
5. It saves the settings and switches to your network within a couple of
   seconds. Rejoin your normal Wi-Fi and check the serial log for the new IP
   address (or check your router's client list / a network scanner) — this
   firmware doesn't do mDNS (`.local` hostnames) yet, so the IP is the only way
   to find it at this stage.
6. Power-cycle the board — it should reconnect to the saved network
   automatically, no setup network this time (Specification §11).

To make it forget the network and go back to setup mode, use the "Forget
Wi-Fi" button on the `/wifi` page (or wipe flash / erase NVS).

## What you should see

**No LEDs wired yet?** The serial log carries the same information the LEDs
would show:

```
[BOOT] Geyser Diverter firmware V0.2 (+ Wi-Fi, local web UI)
[BOOT] Outputs initialized to safe (LOW) default state
[BOOT] Watchdog armed: 5s timeout
[WIFI] No/failed credentials -> AP mode. SSID 'GeyserDiverter-3A1F', http://192.168.4.1/
[WEBUI] Local web server started on port 80
[HEARTBEAT] alive, uptime=0s, fault=no
...
```

Once connected to a real network, `[WIFI] Connected. IP: 192.168.x.x` replaces
the AP-mode line, and `LED_LINK` goes solid.

## Testing the watchdog

Still the one V0.1 behavior worth proving on the bench, not just trusting:

1. Hold the board's **BOOT** button (GPIO0) while it powers up or resets.
2. It logs that the test is armed, runs normally for 5 seconds, then
   deliberately hangs forever — the same failure mode as firmware that's stopped
   responding.
3. Within ~5 seconds the watchdog should reset the board on its own.

## Known simplifications in this stage (not bugs, just not built yet)

- **No mDNS/`.local` hostname.** You need the IP address from the serial log
  or your router until a later stage adds this.
- **No dual AP+STA fallback.** While attempting to (re)connect to a saved
  network, the setup Wi-Fi network is off — if the saved network is wrong or
  unreachable, it falls back to setup mode after the 15s timeout rather than
  staying reachable throughout the attempt. You'll briefly lose the connection
  either way; this just means "briefly" instead of "not at all."
- **Reconnect-after-save is deferred by ~1s** before switching networks, so the
  "saved" confirmation page has time to actually reach your browser before the
  radio switches modes — a known ESP32 web-config gotcha, not an oversight.

## Not built yet

Everything else in the staged plan (Specification §28): PT1000 temperature
(V0.3), CT current sensing (V0.4), manual SSR control (V0.5), inverter RS-485
(V0.6), the surplus-solar algorithm (V0.7), battery protection (V0.8), automatic
control (V0.9), cloud pairing (V0.10), then full fault handling + data logging
(V1.0). Each stage gets the same treatment this one did: build it, bench-test it
on real hardware, confirm it fails safe, *then* move to the next stage — never
all at once, and never near mains power until the stage that actually needs it
says so.
