# WebLink Primary

This is the Web-link board for the primary (transmitter) side. It receives the secondary side voltage/current from the Firebase realtime database, and sends it via i2c to the inverter controller (Teensy 4.1). The board itself is an ESP32 dev module.

> This is created as an Arduino project, as for some (currently unknown) reason the code refused to compile with Platform.io, but readily compiled with Arduino.

## Brief explaination of code
1. Firebase ESP Client library (v4.3.10) by mobizt is used to get data from Firebase realtime database
2. Wire library is used for i2c communication
3. ESP32 is set-up as i2c slave device
4. It waits for a request from the master device (teensy 4.0 inverter controller), and then sends 5 bytes of data:
    1. 2 bytes for voltage
    2. 2 bytes for current
    3. 1 byte for mode
5. The mode byte is used to indicate the mode of operation of the inverter controller. The modes are:
    1. 0: OFF
    2. 1: STANDBY
    3. 2: ACTIVE
    4. 3: OFFLINE

