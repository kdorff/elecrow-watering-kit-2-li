/**************************************************
 * This code exists to obtain sensor values
 * from the mostire sensors and the ToF sensor
 * in order to aid with calibration.
 * 
 * With the moisture sensors, we can capture
 * values that are 100% wet and 0% wet to
 * plug into watering-kit-config.ino to have more
 * accurate moisture levels.
 * 
 * With the ToF sensor, this can be helpful
 * in determining the "known depth" of the
 * reservoir. 
 * 
 * This code has been tested with the Elecrow
 * watering kit that has an integrated 
 * Arduino Leonardo.
 * 
 * Make sure to set your Board and Port 
 * appropriatly. See the README.md for programming
 * notes.
 **************************************************/

#include <Wire.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <RTClib.h>
#include <VL53L0X.h>

// The value returned by the TOF sensors when a timout occurs
// You probably don't want to change this unless you are using
// a different sensor.
#define WATER_LEVEL_TIMEOUT 65535

U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
RTC_DS1307 RTC;

// The number of sensors. If you want more, you will need
// to many of the below arrays, too.
int num_sensors = 4;

// set all moisture sensors PIN ID
int moisture_pins[] = {A0, A1, A2, A3};

// declare moisture values
int moisture_values[] = {0, 0, 0, 0};
int moisture_values_max[] = {0, 0, 0, 0};
int moisture_values_min[] = {32767, 32767, 32767, 32767};

// Water level
bool water_level_enabled = false;
VL53L0X water_level_sensor;
uint16_t water_level_mm = 0;

void setup()
{
  Wire.begin();
  RTC.begin();

  Serial.begin(19200);
  
  u8g2.begin();

  setup_water_level_sensor();
}

void setup_water_level_sensor()
{
  water_level_enabled = false;
  water_level_sensor.setTimeout(500);
  if (water_level_sensor.init())
  {
    water_level_enabled = true;
    // High accuracy - increase timing budget to 200 ms
    water_level_sensor.setMeasurementTimingBudget(200000);
  }
}

void loop()
{
  // read the value from the moisture sensors:
  read_value();
  check_water_level();
  u8g2.firstPage();
  do
  {
    draw_stats();
  } while (u8g2.nextPage());
  delay(500);
}

//Set moisture value
void read_value()
{
  /**************These is for resistor moisture sensor***********
  float value = analogRead(A0);
  moisture_values[i] = (value * 120) / 1023; delay(20);
  ...
 **********************************************************/
  /************These is for capacity moisture sensor*********/
  for (int i = 0; i < num_sensors; i++)
  {
    float value = analogRead(moisture_pins[i]);
    // Keep the int portion, no need for the decimal portion
    moisture_values[i] = value;
    moisture_values_max[i] = max(moisture_values[i], moisture_values_max[i]);
    moisture_values_min[i] = min(moisture_values[i], moisture_values_min[i]);
    
    delay(20);
  }
}

void check_water_level()
{
  if (water_level_enabled)
  {
    // This will return WATER_LEVEL_TIMEOUT if a timeout happens.
    water_level_mm = water_level_sensor.readRangeSingleMillimeters();
  }
}

void draw_stats()
{
  int x_offsets[] = {0, 32, 64, 96};
  char display_buffer[6] = {0};

  u8g2.setFont(u8g2_font_8x13_tr);
  u8g2.setCursor(9, 60);
  u8g2.print("W. LEVEL");
  if (!water_level_enabled)
  {
    u8g2.drawStr(x_offsets[2] + 16, 60, "N/C");
  }
  else
  {
    itoa(water_level_mm, display_buffer, 10);
    u8g2.drawStr(x_offsets[2] + 16, 60, display_buffer);
  }

  for (int i = 0; i < num_sensors; i++)
  {
    itoa(moisture_values[i], display_buffer, 10);
    u8g2.drawStr(x_offsets[i] + 2, 45, display_buffer);
    
    itoa(moisture_values_min[i], display_buffer, 10);
    u8g2.drawStr(x_offsets[i] + 2, 30, display_buffer);
    
    itoa(moisture_values_max[i], display_buffer, 10);
    u8g2.drawStr(x_offsets[i] + 2, 15, display_buffer);
  }
}
