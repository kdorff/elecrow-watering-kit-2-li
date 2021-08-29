# Custom Firmware for Elecrow Watering Kit 2.1

I purchased the [Elecrow Watering Kit 2.1](https://www.elecrow.com/arduino-automatic-smart-plant-watering-kit.html)
from [Amazon](https://www.amazon.com/Elecrow-Watering-Moisture-Gardening-Automatic/dp/B07LCNKC6N). 
The controller board for this kit has an integrated
Arduino Leonardo.

The included firmware (and the firmware found on the Elecrow site) has issues, notably graphical gitches. I've included a copy of the Elecrow code in the `orig/` folder for comparison purposes.

I found a [version of the firmware](https://github.com/liutyi/elecrow-watering-kit-2-li)
modified by [liutyi](https://wiki.liutyi.info/display/ARDUINO/Arduino+Automatic+Smart+Plant+Watering+Kit+2.0a) 
that fixed these gitches. 

Changes to my code
* Formatting and variable names made more consistent
* This supports the VL53L0X Time-of-Flight (ToF) Laser Ranging Sensor I2C IIC module.
* Sensor data is sent over TX to an ESP8266 (if available) running the `esp8266-app/esp8266-app.ino`. I use a D1 Mini (clone) for this
* The ESP8266 I connect VCC and GND from ... and I connect RX on the Elecrow (which is mis-labeled) to RX on the D1 Mini.
* This will preclude you from using the Serial monitor on the D1 Mini. I do not think there is a way around this. Since I am using the RX pin, I do not believe you need to use a level shifter (since it has a resistor in-place).
* Code on the ESP8266 connects to a Local Mosquitto. From there you need to get the data wherever makes sense
* My code assumes you have your Serial consoles set to 19200.

Calibration
* If you flash the moisture-calibratino firmware, you can
  determine the "WET_VALUE" and "DRY_VALUE" for your sensors
  to make sure you are covering the complete range.
* Run the app on the Elecrow hardware with the Moisture sensors
  (and optionally the ToF sensor installed).
* For each moisture sensor observe minimum values for wet and
  maximum values for dry. Adjust WET_VALUE and DRY_VALUE in
  watering-kit-config.h accordingly.
* If using the ToF sensor to check the depth of your
  water reservoir, this will show you distance in
  millimeters. Affix your sensor over your reservoir
  and then get the reading from the Elecrow display. Adjust
  MAX_WATER_DEPTH in watering-kit-config.h to a value LESS
  than this (you don't want the water to actually reach the
  sensor at "Full").

Programming notes
* I found programming this Leonardo to bit a bit of a pain.
* On Windows, the Leonardo consumes two COM ports, let's say they are COM5 and COM6
* COM5 is for the Serial Monitor, when running a program. 
* COM6 is for programming.
* To program, my recommendation
  * Make sure Show Verbose Output on "Upload" is selected. This is helpful!!
  * Click "Compile"
  * Make sure COM6 is selected (if not listed, press and hold the RESET button on the Elecrow)
  * Click and hold the reset button
    * Click "Send"
    * Wait for the build to ALMOST finish and let go of RESET
    * When you start getting messages listing all of the COM ports, press RESET again (don't hold)
    * Programming should start (and complete)
* To view the Serial console, first switch to COM5 (re-open the Serial Monitor, if necessary)
