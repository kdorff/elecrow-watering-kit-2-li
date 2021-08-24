/**************************************************
 * Add your Wifi detils and other config in esp8266-app-config.h
 * (See esp8266-app-config.h-sample).
 * Review #defines and variables at the top of this file.
 * 
 * This code assumes anonymous MQTT access. Change accordingly.
 * 
 * This code has been tested with an ESP8266 D1 Mini Clone. 
 * Make sure to set your Board and Port appropriatly.
 **************************************************/
#include <WiFiServerSecure.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// Review all of the values found here.
// See esp8266-app-config.h-sample
#include "esp8266-app-config.h"

// Pin values defined in esp8266-app-config.h
SoftwareSerial waterSerial(RX_PIN, TX_PIN);

// Change the credentials below, so your ESP8266 connects to your router
// Values in esp8266-app-config.h
const char *mqtt_server = MQTT_HOST;

// Initializes the espClient. You should change the espClient
// name if you have multiple ESPs running in your home automation system
WiFiClient espwateringClient;
PubSubClient client(espwateringClient);

// Current loop number. Should always increase as we loop()
unsigned long loop_num = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi()
{
    delay(10);
    // We start by connecting to a WiFi network
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        // Attempt to connect
        if (client.connect(MQTT_CLIENT_NAME))
        {
            client.publish(MQTT_ONLINE_TOPIC, "1");
        }
        else
        {
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{
    //
    // NOTE: It doesn't seem the ESP8266 can receive data
    // NOTE: on TX _and_ support the Serial monitor.
    //
    Serial.begin(19200);
    waterSerial.begin(19200);
    
    setup_wifi();
    client.setServer(MQTT_HOST, 1883);
}

void loop()
{
    char waterData[255]; // max line length is one less than this

    if (!client.connected())
    {
        reconnect();
    }

    if (!client.loop())
    {
        client.connect(MQTT_CLIENT_NAME);
        client.publish(MQTT_ONLINE_TOPIC, "2");
    }

    // Publish the loop # for the esp8266-app, verifies
    // the app doesn't restart, etc. mostly for debugging.
    static char output_buffer[10];

    // Debugging, publish the loop number
    // dtostrf(loop_num++, 9, 0, output_buffer);
    // client.publish(MQTT_LOOP_TOPIC, output_buffer);

    //  get data from serial line
    if (read_line(waterData, sizeof(waterData)) < 0)
    {
        client.publish(MQTT_ERROR_TOPIC, "line too long");
        return; // skip command processing and try again on next iteration of loop
    }
    if (strlen(waterData) > 0) {
      // We'll process the data in Node-Red
      if (waterData[0] != '#') {
        // Don't publish if starts with '#'
        client.publish(MQTT_DATA_TOPIC, (const char *) &waterData);
        delay(100);
      }
    }
}

int read_line(char *buffer, int bufsize)
{
    for (int index = 0; index < bufsize; index++)
    {
        // Wait until characters are available
        while (waterSerial.available() == 0)
        {
        }

        char ch = waterSerial.read(); // read next character

        if (ch == '\n')
        {
            buffer[index] = 0; // end of line reached: null terminate string
            return index;      // success: return length of string (zero if string is empty)
        }

        buffer[index] = ch; // Append character to buffer
    }

    // Reached end of buffer, but have not seen the end-of-line yet.
    // Discard the rest of the line (safer than returning a partial line).

    char ch;
    do
    {
        // Wait until characters are available
        while (waterSerial.available() == 0)
        {
        }
        ch = waterSerial.read(); // read next character (and discard it)
    } while (ch != '\n');
    buffer[0] = 0; // set buffer to empty string even though it should not be used
    return -1;     // error: return negative one to indicate the input was too long
}
