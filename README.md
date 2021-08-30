# Custom Firmware for Elecrow Watering Kit 2.1

I purchased the [Elecrow Watering Kit 2.1](https://www.elecrow.com/arduino-automatic-smart-plant-watering-kit.html)
from [Amazon](https://www.amazon.com/Elecrow-Watering-Moisture-Gardening-Automatic/dp/B07LCNKC6N). 
The controller board for this kit has an integrated
Arduino Leonardo.

The included firmware (and the firmware found on the Elecrow site) has issues, notably graphical gitches. I've included a copy of the Elecrow code in the `orig/` folder for comparison purposes.

I found a [version of the firmware](https://github.com/liutyi/elecrow-watering-kit-2-li)
modified by [liutyi](https://wiki.liutyi.info/display/ARDUINO/Arduino+Automatic+Smart+Plant+Watering+Kit+2.0a) 
that fixed these gitches. 


## Changes to my code ##
* Formatting and variable names made more consistent
* This supports the VL53L0X Time-of-Flight (ToF) Laser Ranging Sensor I2C IIC module.
* Sensor data is sent over TX to an ESP8266 (if available) running the `esp8266-app/esp8266-app.ino`. I use a D1 Mini (clone) for this
* The ESP8266 I connect VCC and GND from ... and I connect RX on the Elecrow (which is mis-labeled) to RX on the D1 Mini.
* This will preclude you from using the Serial monitor on the D1 Mini. I do not think there is a way around this. Since I am using the RX pin, I do not believe you need to use a level shifter (since it has a resistor in-place).
* Code on the ESP8266 connects to a Local Mosquitto. From there you need to get the data wherever makes sense
* My code assumes you have your Serial consoles set to 19200.

## Monitoring with MQTT, Node-RED, InfluxDB, and Grafana ##

### Data to MQTT ###
* Assuming the `esp82660-app` is be receiving stats from the
  Elecrow watering kit and can connect to the MQTT broker,
  will publish stats to a topic. The topic I publish to is
  `home/watering-1/data`.
* I use the IOTStack project to run MQTT, Node-RED, 
  InfluxDB, and Grafana. I run on the stack on a Raspberry 
  Pi that uses an M.2 SSD over USB for all storage for 
  increased speed and reliability vs using an SD card.

### Node-RED move the data from MQTT to Influx DB ###

I use a very simple Flow in Node-Red to move my data
from MQTT to InfluxB.

* Node 1 `mqtt in`.  This connects to your Mosquitto
  broker and subscribes to the `Topic` `home/watering-1/data`.
* Node 2 `function node`. The input comes from Node 1.
  The purpose of this function is to take the CSV data from
  MQTT and convert it into a JSON object. This function
  contains the code

```
const data = msg.payload.split(",");
msg.payload = {
    'moisture-0' : parseInt(data[0]),
    'moisture-1' : parseInt(data[1]),
    'moisture-2' : parseInt(data[2]),
    'moisture-3' : parseInt(data[3]),
    'pump-0' : parseInt(data[4]),
    'water-level-0' : parseInt(data[5]),
    'water-level-per-0' : parseInt(data[6]),
    'valve-0' : parseInt(data[7]),
    'valve-1' : parseInt(data[8]),
    'valve-2' : parseInt(data[9]),
    'valve-3' : parseInt(data[10])
};
return msg;
```

* Node 3 `influxdb out`. The input comes from Node 2. This 
  connects to your InfluxDB database. 
  I output to a `measurement` `home/watering-1`.
* Node 4 `debug`. The input also comes from Node 2. I find
  this helpful to view the data as it comes from MQTT
  and gets converted to JSON.

## Displaying Stats ##

Find a simple Grafana tutuorial and make charts from the
data that is now in your InfluxDB within `home/watering-1`. 
I may expand this in the future.

## Calibration ##

* If you flash the moisture-calibration firmware, you can
  determine the `WET_VALUE` and `DRY_VALUE` for your sensors
  to make sure you are covering the complete range.
* Run the app on the Elecrow hardware with the Moisture sensors
  (and optionally the ToF sensor installed).
* For each moisture sensor observe minimum values for wet and
  maximum values for dry. Adjust `WET_VALUE` and `DRY_VALUE` in
  `watering-kit-config.h` accordingly.
* If using the ToF sensor to check the depth of your
  water reservoir, this will show you distance in
  millimeters. Affix your sensor over your reservoir
  and then get the reading from the Elecrow display. Adjust
  `MAX_WATER_DEPTH` in `watering-kit-config.h` to a value LESS
  than this (you don't want the water to actually reach the
  sensor at "Full").

## Programming notes on Windows ##

* I found programming this Leonardo to bit a bit of a pain.
* On Windows, the Leonardo consumes two COM ports, let's say they are COM5 and COM6
* COM5 is for the Serial Monitor, when running a program. 
* COM6 is for programming.
* To program, my recommendation
  * Make sure `Show Verbose Output on Upload` is selected. This is helpful!!
  * Click `Verify`
  * Make sure COM6 is selected (if not listed, press and hold the RESET button on the Elecrow)
  * Click and hold the reset button
    * Click `Send`
    * Wait for the build to ALMOST finish and let go of RESET
    * When you start getting messages listing all of the COM ports, press RESET again (don't hold)
    * Programming should start (and complete)
* To view the Serial console, first switch to COM5 (re-open the Serial Monitor, if necessary)
