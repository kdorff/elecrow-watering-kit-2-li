# Custom Firmware for Elecrow Watering Kit 2.1

I purchased the [Elecrow Watering Kit 2.1](https://www.elecrow.com/arduino-automatic-smart-plant-watering-kit.html)
from [Amazon](https://www.amazon.com/Elecrow-Watering-Moisture-Gardening-Automatic/dp/B07LCNKC6N). 
The controller board for this kit has an integrated
Arduino Leonardo.

This kit was a great starting point but I've since made numerous
changes from the original code.

## Waterproofing ##

I intended to use this kit outside. To that end, I knew I'd
have to waterproof the electronics (I placed them in a plastic
box that had a nice seal on the lid and made small holes for the
cables). I used a similar box for the pump and vavle block.

I quickly discovered the Moisture Probes would need to be
insulated / sealed. I used a piece of 
[3/4" heat shrink tubing](https://www.amazon.com/gp/product/B07MF826Q6) 
along with a piece of 
[1/2" heat shrink tubing](https://www.amazon.com/gp/product/B07HT58WPR)
to enclose the electronics and connector on the
probe. I used a heat gun to shrink the tubing.

I then used 
[Liquid Electrical Tape](https://www.amazon.com/Star-brite-Liquid-Electrical-Tape/dp/B0000AXNOD)
to seal the ends of the heat shrink tubing to provide 
additional water protection.

## Water Connections ##

I found the water connections to the Pump and Vavle
Block had a tendency of separating. I used some E6000
to glue the connections into places.

## Changes to my code ##

* Formatting and variable names made more consistent
* This supports the VL53L0X Time-of-Flight (ToF) Laser Ranging Sensor I2C IIC module.
* Sensor data is sent over TX to an ESP8266 (if available) running the `esp8266-app/esp8266-app.ino`. I use a D1 Mini (clone) for this
* The ESP8266 obtains VCC and GND from port that contains `MOSI` and `MISO`.
* The ESP8266 `RX` pin is connected to the Elecrow's `RX` pin (which is
  actually `TX`, but seems to be mis-labeled).
  * This will preclude you from using the Serial monitor on the ESP8266.
    I do not think there is a way around this. 
  * Since I am using the ESP8266 RX pin, I do not believe you need to 
    use a level shifter (since it has a resistor in-place). If you
    are squeemish, you might with to use a 3.3V/5V level shifter
    here such as the `74AHCT125`.
* Code on the ESP8266 connects to a local Mosquitto broker. 
  From there you can get the data to wherever makes sense. I'm
  using Node-RED to migrate the data to InfluxDB which I can then
  visualize using Grafana.
* Added a `moisture-calibration` app to help calibrate the values
  coming from the moisture sensors.
* MINOR Auto-calibration of the moisture sensors, but you should still
  run `mosisture-calibration` app to get the absolute `wet` and `dry`
  values for your sensors.

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

## Acknowledgements ##

I've included a copy of the Elecrow code in the `orig/` folder for comparison purposes. The firmware that was provided by Elecrow has several
issues, not the least of which are  graphical gitches on the display.

I found a [version of the firmware](https://github.com/liutyi/elecrow-watering-kit-2-li)
modified by [liutyi](https://wiki.liutyi.info/display/ARDUINO/Arduino+Automatic+Smart+Plant+Watering+Kit+2.0a) 
that fixed these gitches. 

My code started it's life based on liutyi's
modifications but have since undergone significant changes.

Additional ideas and code were borrowed from 
[rfrancis97](https://github.com/rfrancis97/elecrow-watering-kit-to-ESP8266)
to use an ESP8266 (or ESP32) to publish stats to an MQTT broker.
rfrancis97 also implemented a way to measure water level using an
an Ultrasonic sensor (HC-SR04). I liked this idea but decided to
use a VL53L0X Time-of-Flight (ToF) Laser Ranging Sensor I2C IIC module,
instead.
