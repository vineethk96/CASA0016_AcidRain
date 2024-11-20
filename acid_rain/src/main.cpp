#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "DFRobot_PH.h"
#include "DHT.h"
#include "DHT_U.h"

#define PH_PIN A0
#define WATERDET_PIN 13
#define DHT_PIN 12
#define NEO_PIN 11

#define DHTTYPE DHT22
#define NEO_LED 8
#define FIVE_SEC 5000
#define TWO_SEC 2000
#define TEN_SEC 10000
#define MAX_ACID 0
#define MIN_ACID 14
#define MAX_HUE 255
#define MIN_RED 41
#define MIN_BLUE 109

#define GREEN_POLY_ARG1 -3.1743
#define GREEN_POLY_ARG2 34.5929
#define GREEN_POLY_ARG3 101.8214

DFRobot_PH ph;
DHT dht(DHT_PIN, DHTTYPE);
Adafruit_NeoPixel litmus_strip(NEO_LED, NEO_PIN);

void setup()
{
  // Start Serial Debugger
  Serial.begin(9600);

  // Start pH Sensor
  ph.begin();

  // Start dht Sensor
  dht.begin();

  // Start Litmus LED Strip
  litmus_strip.begin();

  // Set the pin for the water detection pin
  pinMode(WATERDET_PIN, INPUT);
}

void loop()
{
  static float temp = 0;
  static long lastReadTemp, lastReadpH = millis();
  static bool stripSet = false;

  // Get temperature to use for pH reading every 5 sec
  if (millis() - lastReadTemp > FIVE_SEC)
  {
    temp = dht.readTemperature(false);
    lastReadTemp = millis();
    Serial.print("Temp: ");
    Serial.println(temp);
  }

  // if (digitalRead(WATERDET_PIN))
  // {
  //   Serial.println("WATER DETECT");
  // }
  Serial.println(digitalRead(WATERDET_PIN));

  // Only start taking readings upon water detection
  if (digitalRead(WATERDET_PIN) && (millis() - lastReadpH > TWO_SEC))
  {
    // Check the pH Level of the liquid
    int voltage = analogRead(PH_PIN) / 1024.0 * 5000;
    float phValue = ph.readPH(voltage, temp);

    Serial.print("pH Level: ");
    Serial.println(phValue);

    int red = map(phValue, MAX_ACID, MIN_ACID, MAX_HUE, MIN_RED);
    int blue = map(phValue, MAX_ACID, MIN_ACID, MAX_HUE, MIN_BLUE);
    int green = (GREEN_POLY_ARG1 * pow(phValue, 2)) + (GREEN_POLY_ARG2 * phValue) + GREEN_POLY_ARG3;
    // REPLACE GREEN IF IT DOESN'T WORK

    // Light up the Strip like a Litmus Paper
    for (uint8_t i = 0; i < NEO_LED; i++)
    {
      litmus_strip.setPixelColor(i, red, green, blue);
    }
    litmus_strip.show();
    stripSet = true;
    lastReadpH = millis();
  }
  else
  {
    // Clear LEDs if 10 sec has elapsed since last reading
    if ((millis() - lastReadpH > TEN_SEC) && stripSet)
    {
      litmus_strip.clear();
      litmus_strip.show();
      Serial.println("Clearing Strip");
      stripSet = false;
    }

    // Wait a little bit before checking again
    delay(500);
  }
}