Smart Dual-Gate Security Access Control System

This project is an IoT-based embedded security access control system designed to simulate a secure vehicle entry point, such as a restricted parking area, private facility entrance, or toll access checkpoint.

The system uses a Raspberry Pi as the central controller and web server, while two Arduino microcontrollers handle real-time embedded control of sensors and actuators. Vehicles approaching the entrance are detected using distance sensors, and the driver must authenticate using an RFID access card.

If the RFID credential is authorized, the system opens the first gate using a servo motor, allowing the vehicle to enter a secure holding area. Once the vehicle is detected inside the chamber, the first gate closes behind it, and the second gate opens to allow the vehicle to continue. This dual-gate mechanism simulates a secure access airlock, preventing unauthorized tailgating or forced entry.

If an unauthorized access attempt is detected, the system activates an alarm buzzer, displays an access denied warning on the LCD screen, switches the warning LEDs, and sends an alert to the web dashboard.

A browser-based control dashboard hosted on the Raspberry Pi provides real-time monitoring and manual control of the system. Users can view gate status, sensor states, alarm conditions, access attempts, and remotely trigger functions such as emergency override or manual gate control.

Core Features
RFID-based authentication system
Dual servo-controlled security gates
Vehicle detection using ultrasonic/IR sensors
LCD display for live status messages
Red/green LED access indicators
Buzzer alarm for unauthorized access
Raspberry Pi hosted web dashboard
Real-time monitoring and manual remote control
Serial communication between Raspberry Pi and Arduinos
Embedded state-machine based control logic
Technologies Used
Raspberry Pi
Arduino Uno
Python (Flask for web dashboard)
C/C++ (Arduino programming)
HTML / CSS / JavaScript
RFID module
Servo motors
Ultrasonic sensors
IR sensors
I2C LCD display
LEDs and buzzer
Objective

The goal of this project is to demonstrate concepts in advanced embedded systems, including distributed embedded control, sensor integration, actuator management, real-time communication, IoT connectivity, and web-based remote monitoring in a realistic security automation application. 
