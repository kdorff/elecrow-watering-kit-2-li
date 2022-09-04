## Calibration ##

This program is used to normalize variations in sensors due to manufacturing tolerances.  The values obtained by this program are used in other programs.

To use this calibration tool, do the following:

1. Connect all of the moisture sensors.  They should be dried and away from any moisture (I wipe them gently with a paper towel and set them on a table).
2. Optional: Connect the ToF sensor.  The ToF sensor should be placed in its permanent position above the reservoir.  Fill the resevoir to your desired minimum level (I aim for 10-15% capacity).
3. Flash this `moisture-calibration` firmware to the device.
5. Look at the display.  The top section is a table of values 3 rows tall and 4 columns wide.  Each moisture sensor is represented by a column.  The rows are MAX value, MIN value, and CURRENT value.  The bottom section displays details of the ToF sensor - the detected value is on the right.
4. Once the program starts running, place each sensor in a cup of water up to the line and hold it there for a few seconds, then remove it from the water and set aside on a towel.  As you test each sensor, you should notice the CURRENT and MAX values increase.
5. Next, take note of the values on the display.  Personally, for the moisture sensors I wrote the MIN/MAX values onto the back of the sensors with a permanent marker (above the line).


Apply the calibration data to `watering-kit`
1. Rename the sample file `watering-kit-config-sample.h` as `watering-kit-config.h`
2. Update WET_VALUES to the MAX values from the calibration tool (one value per sensor)
3. Update DRY_VALUES to the MIN values from the calibration tool (one value per sensor)
4. Optional: If using the ToF module, Update MAX_WATER_DEPTH to the ToF sensor value from the calibration tool