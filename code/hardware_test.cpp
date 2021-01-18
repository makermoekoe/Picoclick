/**
 * hardware test for the Picoclick v2.2
 * 
 * This sketch checks if all colors of the WS2812 LEDs are working and if the WiFi is enable to scane for SSIDs. 
 * It visualizes the number of SSIDs found with blinking all LEDs green.
 * Once the LED color cycle has started the Picoclick will shut off by a button press or after beeing on for 16 seconds.
 * 
 * The Picoclick can power itself by holding the latch pin high.
 * Pulling the latch to ground will switch it off.
 * 
 * Use WiFi.mode(WIFI_OFF) & WiFi.forceSleepBegin() in the setup for power saving when you don't wanna use WiFi.
 * 
 * 17.12.2020 by @maker.moekoe (Moritz König)
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

  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(0, 0, 200);
    FastLED.show();
    delay(500);
  } 
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(200, 0, 0);
    FastLED.show();
    delay(500);
  } 
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(0, 200, 0);
    FastLED.show();
    delay(500);
  } 

  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(10, 10, 10);
  } 
  FastLED.show();

  //SSID scan
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int n = WiFi.scanNetworks();
  if(n==0){
    leds[2] = CRGB(200, 10, 10);
    FastLED.show();
    delay(2000);
    leds[0] = CRGB(10, 10, 10);
    FastLED.show();
  }
  else{
    for(int i=0; i<n; i++) {
      for(int i=0; i<NUM_LEDS; i++) leds[i] = CRGB(10, 200, 10);
      FastLED.show();
      delay(200);
      for(int i=0; i<NUM_LEDS; i++) leds[i] = CRGB(10, 10, 10);
      FastLED.show();
      delay(200);
    }
  }
  delay(1000);
  //digitalWrite(latch,LOW);
}

int hue = 0;
void loop() {
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = CHSV(hue % 255, 200, 200);
    FastLED.show();
  }
  if(digitalRead(button)){
    for(int i=0; i<NUM_LEDS; i++){
      leds[i] = CRGB(0, 0, 200);
    }
    FastLED.show();
    delay(100);
    while(digitalRead(button)){
      delay(20);
    }

    leds[0] = CRGB(200, 0, 0);
    leds[1] = CRGB(0, 200, 0);
    leds[2] = CRGB(0, 0, 200);
    FastLED.show();
    delay(1000);
    digitalWrite(latch,LOW);
  }
  hue++;

  if(millis() > 16000){
    digitalWrite(latch,LOW);
    delay(1000);
  }
  
}
