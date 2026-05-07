# Micro_Comms — Wireless Remote Controller for a Wind Turbine

Embedded firmware for a wireless remote controller unit, built as part of the **WEDESIGN** small-scale wind turbine project at the University of Manitoba. The system uses ESP-NOW for low-latency wireless command transport and I²C as the bridge between the wireless front-end and the turbine's motor controller.

Subsystem presented at **ISWTC 2024** (International Small Wind Turbine Competition), Delft, Netherlands.

---

## Overview

The wind turbine needed a way to receive operator commands (start, stop, brake, yaw, pitch adjustments) without a tethered control line. The constraints:

- **Low latency** — operator inputs need to land on the turbine controller fast enough for real-time response
- **Reliable** — drops or duplicate commands could put the turbine in an unsafe state
- **Cheap and simple** — competition project on a student budget, no Wi-Fi infrastructure assumed

The solution: an ESP32 acting as a wireless command receiver using ESP-NOW (Espressif's connectionless peer-to-peer protocol), bridged over I²C to the Arduino Uno that drives the turbine's motor and actuator logic.

```
┌──────────────────┐     ESP-NOW      ┌──────────────────┐     I²C     ┌──────────────────┐
│   ESP32 Sender   │ ───wireless───▶  │  ESP32 Receiver  │ ──────────▶ │  Arduino Uno     │
│  (operator unit) │                  │   (on-turbine)   │             │ (motor / control)│
└──────────────────┘                  └──────────────────┘             └──────────────────┘
```

Splitting wireless and motor-control responsibilities across two MCUs kept the radio interrupt timing isolated from the time-critical motor control loop — a deliberate architectural choice rather than running everything on one chip.

---

## My Role

Original author and primary maintainer. Owned:

- **System architecture** — chose ESP-NOW + I²C split-MCU design, defined the message format and command set
- **ESP32 firmware (sender + receiver)** — wireless transport, peer pairing, command framing
- **I²C bridge** — ESP32-as-master to Arduino-as-slave handoff, including timing and error handling
- **Motor control logic** on the Arduino side — translating received commands into actuator outputs
- **Test benches** — staged verification covering wireless link, I²C transfer, and end-to-end command-to-actuator behavior
- **Onboarding documentation** for new firmware members joining the team

Co-developed with **Syronne Barroso**, who contributed to testing, hardware integration, and protocol validation.

---

## Why This Matters Technically

A few design decisions worth calling out, because they're the kind of thing that comes up in interviews:

- **ESP-NOW over Wi-Fi/Bluetooth** — ESP-NOW has lower setup overhead and lower latency than TCP/IP for short, frequent control messages. No router or pairing dance needed. Trade-off: less mature tooling and no built-in retry semantics, so the application layer has to handle it.
- **Two-MCU split** — could have run everything on the ESP32, but isolating motor PWM/timing on the Arduino meant wireless interrupt jitter couldn't bleed into actuator control. Cheaper than refactoring to a real-time OS.
- **I²C as the bridge** — short physical distance between MCUs, simple to debug with a logic analyzer, no need for a higher-bandwidth bus like SPI for the message rate this system runs at.
- **Staged test benches** — each layer (radio, I²C, command translation, motor output) has its own bench so failures can be isolated. This is the same verification discipline used in regulated embedded work.

---

## Repository Structure

| Directory                                | Purpose                                                              |
|------------------------------------------|----------------------------------------------------------------------|
| `ESPNOW_ESP32_Sender_v4.0/`              | ESP32 firmware for the operator-side controller (transmits commands) |
| `ESPtoArduinoUno_I2C_FromESPNOW_V2.0/`   | ESP32 firmware that receives ESP-NOW commands and bridges them over I²C |
| `I2C_ArduinoReceiver_v2.0/`              | Arduino Uno firmware that receives I²C commands and drives motor logic |
| `Test_Benches/`                          | Isolated test benches for wireless link, I²C transfer, and integrated behavior |
| `ATTRIBUTION.md`                         | Third-party code attribution                                         |

---

## Hardware

- **2× ESP32 dev boards** (one sender, one receiver)
- **1× Arduino Uno** (motor / actuator controller)
- **Motor driver** [add model — e.g., L298N, BTS7960, etc.]
- **Wind turbine subsystem** — motors, encoders, sensors, actuators (developed by the broader WEDESIGN team)
- **Power:** [battery / regulated supply — describe briefly]

---

## How to Build & Run

Each subdirectory is a standalone Arduino IDE / PlatformIO sketch.

### ESP32 Sender (operator side)
```bash
# Open ESPNOW_ESP32_Sender_v4.0/ in Arduino IDE or PlatformIO
# Set the receiver MAC address in the source
# Board: ESP32 Dev Module
# Flash and power from USB or battery
```

### ESP32 Receiver (on-turbine, ESP-NOW → I²C bridge)
```bash
# Open ESPtoArduinoUno_I2C_FromESPNOW_V2.0/
# Board: ESP32 Dev Module
# Wire SDA/SCL to the Arduino Uno's I²C pins (A4/A5)
```

### Arduino Receiver (motor controller)
```bash
# Open I2C_ArduinoReceiver_v2.0/
# Board: Arduino Uno
# Wire I²C from the ESP32 receiver
# Wire motor driver outputs per the sketch's pin mapping
```

### Test Benches
```bash
# Each test bench under Test_Benches/ exercises one layer in isolation:
#  - wireless link (sender ↔ receiver, no motor)
#  - I²C transfer (ESP32 ↔ Arduino, no wireless)
#  - end-to-end command path
```

---

## What I'd Do Differently

- **Add CRC + sequence numbers** to the command messages — ESP-NOW gives you delivery acks but not application-level integrity guarantees. Adding a CRC and monotonic sequence number would catch out-of-order or corrupted commands cleanly.
- **Replace the Arduino Uno with an STM32** — the Uno was fine for the competition but the limited RAM, no hardware float, and 8-bit AVR architecture make scaling the control logic painful. An STM32 with FreeRTOS would consolidate the design.
- **Heartbeat / link-loss safety state** — currently a wireless dropout doesn't put the turbine into a defined safe state. A heartbeat from the sender with a watchdog on the receiver would make link loss → auto-brake explicit.
- **Move test benches under a CI runner** — even with embedded targets, the host-side portions of the test benches could run in GitHub Actions on every commit.

---

## Related Work

This subsystem was part of WEDESIGN's broader wind turbine controller, presented at **ISWTC 2024** in Delft. Subsystem architecture, design trade-offs, and validation outcomes were presented to competition judges as part of the team's overall design submission.

---

## Contributors

- **Matt I.** — Original author and primary maintainer. System architecture, motor control logic, ESP-NOW/I²C integration, overall software design.
- **Syronne Barroso** — Testing, hardware integration, communication protocol validation, system design.

---

## Third-Party Code and Acknowledgements

Portions of this project are based on tutorials and example code by **Rui Santos and Sara Santos** at Random Nerd Tutorials (https://randomnerdtutorials.com).

The original tutorial content and example code remain the intellectual property of their respective authors and are used in accordance with their original licensing terms. Modifications, integrations, and original code written by the repository owner are licensed separately under the MIT License.

See [ATTRIBUTION.md](ATTRIBUTION.md) for full attribution.

---

## License

MIT — see [LICENSE](LICENSE).
