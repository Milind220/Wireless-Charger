# Secondary Side Sensor Reader Device

This is the code responsible for reading the values of the voltage and current sensors connected to the receiver (Rx) coil. It is written to run on an Arduino Nnano (AtMega328P).

This is done because the voltage/current sensors used in this project run on 5V while the WebLink (ESP32 board) runs on 3.3V. Thus the sensors cannot be directly connected to the ESP32 without damaging the microcontroller hardware. Since the signal from the sensors is analog, it cannot be routed through a logic level-shifter either. Instead the Arduino Nano reads from the sensors, and sends the readings to the ESP32 using i2c (through a logic level shifter).

## Setup
It carries out the following tasks:
* Reads analog values from voltage and current sensor using ADC.
* Converts ADC values to actual voltage/current readings.
* Sends the values over i2c (through a logic level shifter) to the secondary side WebLink (ESP32 board).