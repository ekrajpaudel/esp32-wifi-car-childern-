# esp32-wifi-car-for kids-
This project uses a Client-Server (CS) architecture where the ESP32 operates as both a wifi access poing (SoftAP) and a web Server.
A mobile phone connects to the esp32 through the LAN created by SoftAP and loads a controller (user interface).
The mobbile acts as the client, sendiing HTTP requests such as /forward, /reverse, /left, /right.
The esp32 interprets these commands and controls the BO motors using L293D motor driver by  toggling its GPIO pins H/L.

Features
- esp32 works like a wifi router (SoftAP)
- Mobile and esp32 form private LAN (no internet needed)
- Simple joystick UI
- send request using HTTP GET request
- used L293D for motor driver
- fully offline, fast, safe for kids

Logic
esp32    l293d    function  
23        IN1      Left motor forward
22        IN2      Left motor Reverse
19        IN3      Right motor forward
18        IN4      Right motor reverse
