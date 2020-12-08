/**
 * bare minimum for the Picoclick v2.2
 * 
 * This sketch gives you an LED show by a single button press.
 * 
 * The Picoclick can power itself by holding the latch pin high.
 * Pulling the latch to ground will switch it off.
 * 
 * Use WiFi.mode(WIFI_OFF) & WiFi.forceSleepBegin() in the setup for power saving when you don't wanna use WiFi like in this example.
 * 
 * 29.11.2020 by @maker.moekoe (Moritz König)
 * 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define LIPO_MINIMAL 3200

#define NUM_LEDS 3
CRGB leds[NUM_LEDS];

const int button = 12;
const int latch = 13;
const int status_mcp = 4;

bool state = false;

boolean battery_empty = false;
boolean battery_charging = false;
boolean stay_on = false;

int hue = 0;
double pi = 3.141;
double chg_bright = 100;


void setup() {
  pinMode(latch, OUTPUT);
  digitalWrite(latch, HIGH);

  pinMode(status_mcp, INPUT_PULLUP); //read charging status

  pinMode(button, INPUT);
  state = digitalRead(button);

  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  FastLED.addLeds<WS2812, 14, GRB>(leds, 3).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(200);

  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 0);
  FastLED.show();
}


void loop() {
  for (int b = 0; b < 385*3; b++) {
    float s1 = sin(3.141*float(b)/180.0)*150;
    float s2 = sin(3.141*float(b+30)/180.0)*150;
    if(s1<0) s1 = -s1;
    if(s2<0) s2 = -s2;
    leds[0] = CRGB(0, 0, int(s1));
    leds[1] = CRGB(0, 0, int(s1));
    leds[2] = CRGB(0, 0, int(s2));
    FastLED.show();
    delay(8);
  }

  for(int b = 150; b>10; b--){
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, b);
    FastLED.show();
    delay(7);
  }

  digitalWrite(latch, LOW);
  delay(500);
  //never reaching this
}