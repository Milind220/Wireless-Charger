# The *Smart Charge* System

All the code and design files for the wireless charging system that I made for my final year project! This is the capstone project for my undergraduate degree in Energy Science and Engineering!

*Summary*:

A custom built, high power, smart wireless charging system for robotics. The charger transmitter can be controlled from a sleek online dashboard, and the charging mode, output voltage, current, and power can also be monitored. The charger automatically detects the presence of a robot on the charging pad, and switches from STANDBY to ACTIVE charging mode. It also features a closed-loop feedback system to reach the target charging voltage for the robot.

## Contents
* [Inverter-Controller](#inverter-controller) - The code for controlling the inverter.
* [WebLink-Primary](#weblink-primary) - The code for the primary (transmitter) side weblink.
* [WebLink-Secondary](#weblink-secondary) - The code for the secondary (receiver) side weblink. 
* [Secondary-sensor-reader](#secondary-sensor-reader) - The code for the secondary side sensor reader device.
* [Smartcharge-dashboard](#smartcharge-dashboard) - The online control and monitoring dashboard.
* [Hardware](#hardware) - Electronics schematics, gerber files, and STLs.
* [Simulations and Modelling](#simulations-and-modelling) - EasyEDA circuit simulation, and system modelling Excel sheet.
* [Report and Presentation](#report-and-presentation) - my FYP final report and presentation.


## Inverter Controller

The [inverter controller](https://github.com/Milind220/Wireless-Charger/tree/main/Inverter-Controller) is a Teensy 4.1 microcontroller board. It uses 4 precisely timed PWM signals to control an H-Bridge inverter with phase-shift control, which then generates a modified square wave signal that is used to energise the primary (transmitter) coil. It also uses a single digital pin to control the enable/disable function of the MOSFET driver ICs. It also gets signals for the mode to control the inverter from an ESP32 that communicates with it using i2c. 

The Teensy 4.1 was chosen for this task for the following reasons:
* Arduino compatibility, which makes it ideal for rapid development.
* High speed microcontroller. The Teensy's core is a NXP i.MX RT1062 that runs at 600 Mhz.
* Capable of producing very high speed, high resolution, precisely timed and coordinated PWM signals.

The 4 PWM signals are produced as two pairs of complementary signals, with a fixed duty cycle of 50% and frequency of 100 kHz. The two pairs of signals have a variable phase shift (0-90 degrees), and the degree of phase shift controls the output waveform from the inverter. The RT1062's eFlexPWM module also inserts deadtime, so that shoot through doesn't occur between same-side MOSFETs.

A more detailed explaination of the code is given in the Inverter Controller directory. 

## WebLink Primary

The [Primary Side WebLink]() is an ESP32 development board that communicates with the Teensy 4.1 inverter controller via i2c. It obtains the secondary side voltage and current information from the connected Firebase realtime database. This is then used to derive the mode command that it sends to the Teensy. It also sends the Teensy the secondary voltage level, which is used to implement the closed loop control algorithm.

ESP32's are low cost, easy to use, and incredibly powerful (probably overkill for this task). They are also Arduino Compatible, which once again helps with rapid development.

## WebLink Secondary

The [Secondary Side WebLink]() is an ESP32 development board that sends receiver voltage, current levels to the Firebase realtime database. However, this board does not connect directly to the voltage, current sensors. The sensors used operate at a voltage of 5V, and have analog outputs, which makes it likely that they would damage the ESP32s GPIOs if connected directly. Instead, the ESP32 interfaces with an Arduino Nano via i2c, which is instead used to reads the sensors.

## Secondary Sensor Reader

The 

## SmartCharge Dashboard


## Hardware


## Simulations and Modelling


## Report and Presentation
