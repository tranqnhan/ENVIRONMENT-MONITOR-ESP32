# Environment Monitor ESP32

This is a device that measure temperature, CO2 level, and humidity. 

It send the measurements to a Spring Boot server (Environment Monitor Server) and store the data in a PostgreSQL database. 

Wifi provisioning is implemented with Espressif's Network Provisioning library (SECURITY 2).

### Circuit Diagram

![Environment Monitor Circuit Image](docs/ems_circuit.png)

### Physical Prototype

<img src="docs/ems_device.jpg" alt="Physical device" width="300">

Button is used to swap screen between wifi provision display and the measurement display.

#### Video

https://github.com/user-attachments/assets/9f2d29ea-7e60-432a-91cb-5289f5d07df7

### TODO

- Sends measurement data over HTTPS
