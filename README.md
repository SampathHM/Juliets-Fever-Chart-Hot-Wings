# 🐞 Juliet's Fever Chart — Greenhouse Microclimate Mapper

> **A friendly, autonomous IoT companion that senses air quality, maps microclimates, and opens its wings when Juliet grows warm.**

Developed by **Team Hot Wings** 🔥 for the **ITEE Summer Programme 2026** at the **University of Oulu**.

---

## 📌 Project Overview

In the University of Oulu Botanical Garden, the **Juliet** and **Romeo** greenhouses house delicate, rare tropical plants that depend on stable environmental conditions. However, large greenhouses suffer from hidden **microclimates** — localized warm pockets, cold drafts, and damp zones that central thermostats cannot detect.

**Juliet's Fever Chart** is a portable, standalone IoT ladybug companion designed to detect and visually communicate these microclimate variations in real-time.

### Key Capabilities

- **Visual Mood & Temperature Display** — An OLED display alternates every 2 seconds between an expressively animated face (Happy/Cool vs. Panic/Hot) and a large-font live temperature readout.
- **Kinetic Expressiveness** — Dual servo-driven 3D-printed wings flap in opposing motion when ambient temperature exceeds a configurable threshold.
- **On-the-Fly Calibration** — Users can adjust the base temperature threshold in ±0.5°C increments using antennae-mounted push buttons, without requiring a laptop connection or code upload.
- **Acoustic Alerts** — A passive buzzer triggers a triple-beep notification when temperature crosses the active threshold.
- **Cloud Telemetry** — Live temperature and humidity data are published every 60 seconds over MQTT to the **GardenSpine** cloud dashboard.

---

## 🛠️ Hardware Architecture & Wiring

The device is housed inside a custom **3D-printed ladybug shell** (red/black) mounted on a **laser-cut base**, with a dedicated internal power supply module to deliver stable current to the high-draw servo motors.

```
                  +-----------------------+
                  |    ESP32 Dev Board    |
                  +-----------+-----------+
                              |
     +------------------------+------------------------+
     |                        |                        |
[I2C Pins]               [GPIO Pins]              [Servo Pins]
SDA -> GPIO 21           DHT11 Data -> GPIO 14    Servo 1 (Left)  -> GPIO 13
SCL -> GPIO 22           Buzzer     -> GPIO 27    Servo 2 (Right) -> GPIO 12
(0.96" SSD1306 OLED)     Btn UP     -> GPIO 16
                         Btn DOWN   -> GPIO 17
```

### Complete Pin Mapping Table

| Component | ESP32 Pin | Logic / Type | Description |
|---|---|---|---|
| **OLED SDA** | `GPIO 21` | I2C Data | 0.96" SSD1306 Display (128×64) |
| **OLED SCL** | `GPIO 22` | I2C Clock | Display clock line |
| **DHT11 Sensor** | `GPIO 14` | Digital Input | Temperature & Humidity Sensor |
| **Servo 1 (Left Wing)** | `GPIO 13` | PWM Output | Servo motor for left wing swing |
| **Servo 2 (Right Wing)** | `GPIO 12` | PWM Output | Servo motor for right wing swing |
| **Buzzer** | `GPIO 27` | GPIO Output | Passive piezo buzzer for tone generation |
| **Antenna Button 1 (UP)** | `GPIO 16` | Digital Input (`INPUT_PULLUP`) | Increases base threshold by +0.5°C |
| **Antenna Button 2 (DOWN)** | `GPIO 17` | Digital Input (`INPUT_PULLUP`) | Decreases base threshold by −0.5°C |

---

## 💻 Software Architecture

The firmware is written in C++ for the Arduino / ESP32 framework and built on top of the **GardenSpine** telemetry library.

### Non-Blocking Multitasking Design

Instead of using restrictive `delay()` calls that lock up the system, the loop uses a `millis()` timing scheduler to coordinate:

1. **Servo Wing Motion** — Runs continuously without interruption when `currentTemp > BASE_TEMP`.
2. **OLED Frame Animation** — Updates eye-blink animations every 200 ms across a 25-frame cycle.
3. **Display Mode Switch** — Toggles between animated face and big numerical readout every 2000 ms.
4. **DHT11 Sampling** — Reads temperature and humidity every 5000 ms to avoid sensor thermal drift.
5. **MQTT Cloud Publishing** — Sends metrics to `gardenspine.ikapo.fi` every 60 000 ms.

---

## 🚀 Setup & Installation

### Prerequisites

1. **Arduino IDE 2.x** with ESP32 board support installed.
2. Required Libraries (install via Arduino Library Manager):
   - `GardenSpine`
   - `ESP32Servo`
   - `DHT sensor library` (Adafruit)
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`

### Installation Steps

1. **Clone / Fork this repository**

   ```bash
   git clone https://github.com/YOUR_USERNAME/juliet-fever-chart.git
   ```

2. **Configure Credentials**

   Copy `config.h.example` to `config.h` in the main sketch directory:

   ```bash
   cp config.h.example config.h
   ```

   Open `config.h` and fill in your WiFi and GardenSpine MQTT credentials:

   ```cpp
   const char* SPINE_WIFI_SSID = "panoulu";
   const char* SPINE_WIFI_PASSWORD = "";
   const char* SPINE_DEVICE_ID = "gm-01"; // Your assigned ID
   const char* SPINE_MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";
   ```

   > ⚠️ **Security Warning:** Never commit your populated `config.h` file to Git!

3. **Upload Firmware**

   Select your ESP32 board (e.g. *ESP32 Dev Module*) and COM port in Arduino IDE, then click **Upload**.

---

## 🛰️ GardenSpine Telemetry Integration

Data is transmitted securely over TLS-encrypted MQTT (Port `8883`) using the official GardenSpine data contract:

```cpp
spine.publish("temperature", currentTemp, "celsius");
spine.publish("humidity", currentHumidity, "percent-rh");
```

| Property | Value |
|---|---|
| **Target Host** | `gardenspine.ikapo.fi` |
| **Topic Format** | `garden/<SPINE_ZONE>/<SPINE_DEVICE_TYPE>/<SPINE_DEVICE_ID>/<measurement>` |
| **Interval** | Every 60 seconds |

---

## 🕹️ Interactive Controls & User Guide

| Trigger | System Reaction |
|---|---|
| **Press Antenna UP Button (`GPIO 16`)** | Displays current threshold → increases `BASE_TEMP` by **+0.5°C** → displays new threshold |
| **Press Antenna DOWN Button (`GPIO 17`)** | Displays current threshold → decreases `BASE_TEMP` by **−0.5°C** → displays new threshold |
| **`Temperature ≤ BASE_TEMP`** | Display shows happy "Cool" face with blinking eyes, alternating with numerical readout. Wings remain resting at `90°` |
| **`Temperature > BASE_TEMP`** | Display shows angry "HOT!" face alternating with numerical readout. Wings flap in opposing direction (`70°` ↔ `110°`) |
| **Crossing Threshold Line** | Buzzer plays a 3-pulse audible beep alert |

---

## 🔮 Future Improvements

1. **Locomotion / Autonomous Mapping** — Add motorized wheels so the ladybug can patrol greenhouse aisles independently and generate 2D heat maps.
2. **Actuator Switching** — Integrate a smart WiFi relay (e.g. Tuya / Tasmota) to wirelessly turn ON/OFF AC-powered fans or heating pads when alerts trigger.
3. **Swarm Intelligence** — Deploy multiple ladybug companions across the greenhouse to build a comprehensive real-time microclimate grid on GardenSpine.

---

## 👥 Team Hot Wings

| Member |
|---|
| **Sampath Sujeewa Herath** |
| **Kalyani Mukherjee** |
| **Mai Truong** |

*Project developed during the ITEE Summer Programme 2026, University of Oulu, Finland.*

---

## 📜 License & Security Rules

- Never commit `config.h` or any file holding credentials.
- Never use `client.setInsecure()`. A certificate error is a real error.
- Never publish over plaintext port `1883`.
- Never publish under another team's device prefix.

---

## 🔗 Related Links

- [GardenSpine Programme Site](https://gardenspine.ikapo.fi/)
- [Project Charter — Greenhouse Microclimate Mapper](https://gardenspine.ikapo.fi/projects/greenhouse-microclimate-mapper/)
- [Setup Guide](https://gardenspine.ikapo.fi/setup)
- [Data Contract Spec](https://gardenspine.ikapo.fi/spec)
``GLOBAL_FINISH``