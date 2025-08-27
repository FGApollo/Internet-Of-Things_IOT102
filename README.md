# 🚪 IoT Smart Home

This project is an **IoT-based Smart Access Control and Safety System** combining **Arduino UNO + ESP8266** with multiple sensors and actuators.  
It focuses on **RFID-based authentication**, **gas (CO) detection**, **obstacle/motion detection**, and **web-based monitoring**.

---

## ⚙️ Hardware Components
- **Arduino UNO** (RFID reader, Servo motor, LCD)
- **ESP8266 NodeMCU** (Web server + WiFi connectivity)
- **RC522 RFID Reader** (card authentication)
- **MQ-7 Gas Sensor** (CO detection)
- **PIR Sensor** (motion detection)
- **Obstacle Sensor (IR)** (detect objects near the door)
- **16x2 LCD** (system messages)
- **Servo Motor** (door lock mechanism)
- **Buzzer + LED** (alert system)

---

## 🖥️ System Architecture
- **Arduino UNO**
  - Reads RFID card for access control.
  - Controls the **servo motor** to lock/unlock the door.
  - Displays messages on the **LCD** (e.g., "Welcome", "Invalid Card", "Quet the...").
  - Listens to **ESP8266 alerts** (e.g., obstacle detected).
  
- **ESP8266 (NodeMCU)**
  - Hosts a **Web Server** to monitor sensors in real time.
  - Reads **MQ-7 (gas)**, **Obstacle sensor**, and **PIR motion sensor**.
  - Activates **buzzer** for 3 seconds when danger is detected.
  - Sends messages (e.g., `OBSTACLE:DETECTED`) back to Arduino via Serial.
  - Provides a **Web UI** to:
    - Toggle light (GPIO control).
    - Show sensor values & system alerts.

---

## 🚨 Features
- **RFID Authentication**  
  - Only valid UID cards can unlock the door.  
  - Invalid cards trigger an LCD warning.  

- **Door Locking Mechanism**  
  - Controlled by a **servo motor** (90° open, 180° closed).  

- **Obstacle Detection**  
  - Detects objects blocking the door.  
  - Shows message on LCD: *"Co Chuyen Dong"*.  

- **Gas (CO) Detection**  
  - MQ-7 sensor detects CO concentration.  
  - Alerts via buzzer + web interface.  

- **PIR Motion Detection**  
  - Lights up an LED when motion is detected.  

- **Web Dashboard (ESP8266)**  
  - Real-time sensor values.  
  - Remote control of light (ON/OFF).  
  - Danger alerts highlighted in **red**.  

---

## 🌐 Web UI Example
The ESP8266 hosts a simple web server:

- Shows **Light state (ON/OFF)**  
- Displays **MQ-7 gas sensor value**  
- Displays **Obstacle sensor status**  
- Shows **danger alerts in red**

---

## 🖼️ Demo Diagram
