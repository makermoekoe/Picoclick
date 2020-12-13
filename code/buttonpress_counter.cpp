/**
 * read button as interrupt for the Picoclick v2.2
 * 
 * This sketch counts button presses and flashes the LEDs as often as the button was pressed.
 * For interrupt use of ESP8266, ESP8285 and ESP32 you have to call: #define ISR_PREFIX ICACHE_RAM_ATTR 
 * https://github.com/platformio/platform-espressif8266/issues/160
 * 
 * The Picoclick can power itself by holding the latch pin high.
 * Pulling the latch to ground will switch it off.
 * 
 * Use WiFi.mode(WIFI_OFF) & WiFi.forceSleepBegin() in the setup for power saving when you don't wanna use WiFi.
 * 
 * 12.12.2020 by @maker.moekoe (Moritz König)
 * 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

#define ISR_PREFIX ICACHE_RAM_ATTR

#define NUM_LEDS 3
CRGB leds[NUM_LEDS];

const int button = 12;
const int latch = 13;
const int status_mcp = 4;

volatile byte interruptCounter = 0;
volatile unsigned long t_press = millis();

volatile boolean first_press = true;
volatile unsigned long last_interrupt_time = 0;

ISR_PREFIX void handleInterrupt() {
  if (millis() - last_interrupt_time > 70 || first_press){
    interruptCounter++;
    first_press = false;
  }
  last_interrupt_time = millis();
  t_press = millis();
}

void setup() {
  pinMode(latch, OUTPUT);
  digitalWrite(latch, HIGH);

  pinMode(button, INPUT);
  attachInterrupt(digitalPinToInterrupt(button), handleInterrupt, FALLING);

  pinMode(status_mcp, INPUT_PULLUP); 

  FastLED.addLeds<WS2812, 14, GRB>(leds, 3).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(200);

  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(200, 0, 0);
  FastLED.show();
}

void loop() {
  if(millis() > t_press + 500) {
    for(int i=0; i<interruptCounter; i++){
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 200, 0);
      FastLED.show();
      delay(200);
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 0);
      FastLED.show();
      delay(200);
    }
    digitalWrite(latch,0);
    delay(200);
  }
}