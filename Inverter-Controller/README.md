# Inverter Controller

This is a Teensy 4.1 that is used to control an H Bridge inverter using the PWM outputs. It also communicates with the WebLink Primary board via i2c, and adjusts it's mode of operation according to commands from there.

## General Code explaination

1. Configured as i2c master device.
2. Requests a tranmission from the ESP32 Primary Weblink (slave device)
3. Reads the i2c bytes sent from the slave. 5 bytes are sent:
    1. secondary side voltage (2 bytes)
    2. secondary side current (2 bytes)
    3. mode of operation (1 byte)
4. Switches to the appropriate mode of operation. Different mode functions are defined in the code, and are called from the loop function.
5. The eFlexPWM library is used to control the PWM outputs, and generate two pairs of complementary PWM with deadtime and variable phase shift, at a 100 kHz frequency. These are used to drive the H Bridge inverter.

