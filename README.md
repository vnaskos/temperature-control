# Temperature Control (ATmega16)

## Description

This project works as a temperature contoller (Proof Of Concept). An ATmega16 microcontroller reads the temperature from two sensors and regulates the speed of a fan accordingly.

__Work objective.__ Increase the knowledge of programming of serial interfaces on ATmega16 microcontroller in the field of embedded systems.

## Parts Used

* ATmega16
* TC74 temperature sensor (TWI)
* LM35 temperature sensor (Analog)
* Push buttons
* LCD screen
* DC motor
* L293D motor driver

## Tasks

1. The temperature, which was recorded with TC74 digital temperature sensor, should be transmitted to the microcontroller through the TWI serial interface and displayed on the LCD screen in the first line.
2. The temperature, which was recorded with LM35 analog temperature sensor should be transmitted to the microcontroller and displayed on the LCD screen in the second line.
3. It must be possible to control the speed of DC fan motor in manual mode with two buttons. You can choose the step between different speeds independently. The fan motor should work at least in three different speeds.
4. The DC fan motor should also have the automatic control mode. The button No. 3 should allow changing the DC fan motor control mode: manual or automatic. Selected control mode should be displayed on the LCD screen (LCD screen may contain just the first letter: M - manual, A - automatic).
5. The DC fan motor has to work in two different speeds in the automatic mode depending on the temperature of TC74 sensor: 0–10 °C - motor does not rotate; 11–20 °C - the first speed; > 20 °C - the second speed.
6. Temperatures of the LM35 and TC74 sensors should be compared all the time. When temperatures from both sensors varies more than 5 °C, the "System Error" message should be displayed on LCD screen. When the difference in temperature is reduced to less than 5 °C, the LCD should show parameters which were discussed in tasks 1–5 again.
