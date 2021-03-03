/**
 * read button as interrupt for the Picoclick v2.2
 *
 * This sketch counts button presses and flashes the LEDs as often as the button was pressed with the color green.
 * Additionally it visualizes press-and-hold actions by flashing the LEDs in red.
 * So if the button was pressed three times and the last one was held, so LEDs will flash three times in red.
 * For interrupt use of ESP8266, ESP8285 and ESP32 you have to call: #define ISR_PREFIX ICACHE_RAM_ATTR
 * https://github.com/platformio/platform-espressif8266/issues/160
 *
 * The Picoclick can power itself by holding the latch pin high.
 * Pulling the latch to ground will switch it off.
 *
 * Use WiFi.mode(WIFI_OFF) & WiFi.forceSleepBegin() in the setup for power saving when you don't wanna use WiFi.
 *
 * 02.03.2021 by @maker.moekoe (Moritz König)
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
bool longpress = false;

volatile boolean first_press = true;
volatile unsigned long last_interrupt_time = 0;

ISR_PREFIX void handleInterrupt() {
  if (millis() - last_interrupt_time > 70 || first_press){
    interruptCounter++;
    if(interruptCounter == 10) interruptCounter = 9;
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

  FastLED.addLeds<WS2812B, 14, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  delay(1);
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 200);
  FastLED.show();
}

void loop() {

  if(digitalRead(button)){
    unsigned long t = millis();
    while(digitalRead(button) && interruptCounter < 10){
      if(millis() > t + 700){
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(200, 0, 0);
        FastLED.show();
        interruptCounter = interruptCounter * 10;
      }
    }
  }

  if(millis() > t_press + 500) {
    if(interruptCounter >= 10) {
      longpress = true;
      interruptCounter=interruptCounter/10;
    }
    for(int i=0; i<interruptCounter; i++){
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(longpress*200, (!longpress)*200, 0);
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
