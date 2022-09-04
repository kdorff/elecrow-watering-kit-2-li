/**************************************************
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

U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
RTC_DS1307 RTC;

char days_of_the_week[7][12] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

static char output_buffer[10];

void setup()
{
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  u8g2.begin();
  u8g2.firstPage();
  do
  {
    draw_time();
  } while (u8g2.nextPage());
}

void loop()
{
  u8g2.firstPage();
  do
  {
    draw_time();
  } while (u8g2.nextPage());
  delay(500);
}

void draw_time()
{
  int x = 5;
  float i = 25.00;
  float j = 54;
  DateTime now = RTC.now();
  Serial.print(now.year(), DEC);
  u8g2.setFont(u8g2_font_7x13_tr);

  char datestr[32]; //make this big enough to hold the resulting string
  snprintf(datestr, sizeof(datestr), "%4d-%02d-%02d  [%s]", now.year(), now.month(), now.day(), days_of_the_week[now.dayOfTheWeek()]);
  u8g2.setCursor(5, 11);
  u8g2.print(datestr);

  char timestr[9];
  snprintf(timestr, sizeof(timestr), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  u8g2.setCursor(35, 33);
  u8g2.print(timestr);
}