# Smart Security Gate (Team B5)

A smart parking access system built for the Advanced Embedded Systems Lab.
A driver taps an RFID card, and if it's approved and a vehicle is present,
the gate opens, the parking count updates, and everything is shown on a
live web dashboard.

## How it works

- An **ESP32** runs the gate: it reads the RFID card, checks if it's allowed,
  moves the servo gate, controls the LEDs and buzzer, shows messages on an
  LCD, and keeps count of free parking spaces.
- A **Raspberry Pi** runs a web dashboard and an MQTT broker.
- The ESP32 and the Pi talk wirelessly over MQTT.

## Main components

- ESP32 DevKitC (main controller)
- RC522 RFID reader
- Ultrasonic sensor (vehicle detection)
- Servo motor (gate arm)
- Dual-colour LED + buzzer (feedback)
- 16x2 LCD display
- Raspberry Pi (dashboard + MQTT broker)

## Repository structure

- `esp32/` — the Arduino code that runs on the ESP32
- `raspberry-pi/` — the Flask dashboard (`app.py` and `templates/index.html`)
- `docs/` — the project documentation

## Setup notes

- The ESP32 code needs your Wi-Fi name and password added before uploading.
- The Raspberry Pi needs Flask, paho-mqtt, and the Mosquitto broker installed.
- Open the dashboard at the Pi's IP address on port 5000.

## Team

- Muji — coordination and documentation
- Victor — hardware integration
- Jaleel — embedded software
- Umer — communication and testing
