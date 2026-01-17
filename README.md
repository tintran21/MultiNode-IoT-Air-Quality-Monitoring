# 🌫️ Multi-node Air Monitoring System

## Introduction
This repository contains the source code for a **Multi-node Air Monitoring System**, designed with a clear functional separation between nodes:

- **Sensor Node** – environmental data acquisition  
- **Master Node** – central processing and control  
- **Actuator Node** – display and output execution  

The system consists of **three independent nodes** communicating via **WiFi TCP** and **UART**, suitable for indoor air monitoring, laboratory experiments, and IoT learning projects.

---

## System Architecture

- **Sensor Node (ESP32-C3)**  
  Reads environmental sensor data and sends it to the Master Node via **WiFi TCP**.

- **Master Node (ESP32-WROOM-32)**  
  Acts as the central controller:
  - Receives data from the Sensor Node
  - Processes control logic and threshold conditions
  - Sends commands via **UART** to the Actuator Node
  - Hosts a local web interface for monitoring

- **Actuator Node (Arduino Nano)**  
  Executes commands only (no logic processing):
  - Displays data on LCD
  - Controls LEDs and buzzer alarms
  - Drives a fan through a relay

📷 **SYSTEM OVERVIEW**

<img width="1290" height="580" alt="image" src="https://github.com/user-attachments/assets/e509b2b9-e7e4-4f46-b32c-03623fe9767c" />


## Hardware Components

### Microcontrollers
- **ESP32 DevKit V1 (ESP32-WROOM-32, 38 pins)** – Master Node  
- **ESP32-C3 Dev Module** – Sensor Node  
- **Arduino Nano (ATmega328P)** – Actuator Node  

### Environmental Sensors
- **DHT22** – temperature and humidity measurement  
- **MQ-2** – gas detection (LPG, propane, methane, smoke)  
- **MQ-135** – air quality monitoring (NH₃, CO₂, smoke, toxic gases)  
- **LDR** – ambient light intensity measurement  

### Actuators & Display
- 20x4 I2C LCD
- 2 buzzers
- 3 LEDs with 220Ω resistors
- 1 DC fan
- 1 relay module (5V)

### Power Supply
- Battery or external adapter  
- 5V and 3.3V step-down regulators 

---

## GPIO Configuration

📷 **SENSOR NODE GPIO**  

<img width="608" height="395" alt="image" src="https://github.com/user-attachments/assets/401b7c3d-cb08-4540-a13e-a1e20bf2d191" />


📷 **MASTER & ACTUATOR NODE GPIO**

<img width="1006" height="411" alt="image" src="https://github.com/user-attachments/assets/e64b37d3-c47b-471c-9831-50815e7d1d85" />

---

## Operating Principle

- **DHT22**
  - Measures temperature and humidity
  - Data is displayed on the LCD
  - If temperature > 30°C, the cooling fan is activated

- **MQ-2 (Gas Detection)**
  - Detects flammable gases
  - When gas is detected:
    - LEDs blink
    - Buzzer alarm sounds
    - Fan is activated
    - Warning message is shown on the LCD

- **MQ-135 (Air Quality)**
  - Detects smoke, NH₃, and air pollutants
  - When threshold is exceeded:
    - LEDs blink
    - Buzzer alarm sounds
    - Fan is activated
    - Alert is displayed on the LCD

- **LDR**
  - Measures ambient light intensity
  - If light level crosses a defined threshold, LEDs are turned on

- **Web Monitoring**
  - A phone or laptop connects to the WiFi Access Point hosted by the Master Node
  - The local web interface allows:
    - Real-time sensor value monitoring
    - Visualization of temperature and MQ-135 trend graphs

---

## Demo
🔗 **Demo Link:** *[(Click Here)](https://drive.google.com/file/d/10mCTWqZQtkh6QwptWKaQlUsgt7LEzdDk/view?usp=sharing)*

---

## Installation & Setup

### 1. Clone the repository

- git clone https://github.com/username/multi-node-air-monitoring.git

- Or download the source code manually.

## 2. Development Environment

- Visual Studio Code: https://code.visualstudio.com/

- PlatformIO Extension for VS Code: https://platformio.org/install/ide?install=vscode

## 3. Libraries

- Check and install required libraries listed in platformio.ini

- Ensure correct board selection for each node:

- ESP32-WROOM-32

- ESP32-C3

- Arduino Nano

## 📬 Contact

Email: trantin2114@gmail.com

LinkedIn: https://linkedin.com/in/wthtintran/
