/**
 * mqtt example for the Picoclick v2.2
 * 
 * This sketch publishes a simple mqtt message by a single button press.
 * 
 * The Picoclick can power itself by holding the latch pin high.
 * Pulling the latch to ground will switch it off.
 * 
 * Use WiFi.mode(WIFI_OFF) & WiFi.forceSleepBegin() in the setup for power saving when you don't wanna use WiFi.
 * 
 * 29.11.2020 by @maker.moekoe (Moritz König)
 * 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

const char* ssid = "XXX";
const char* password = "XXX";
const char* mqtt_server = "192.168.2.103";
const uint8_t bssid[6] = {0x80, 0x71, 0x7A, 0x3F, 0x8B, 0x63}; //put your access points bssid here
const int wifi_channel = 9; //set the channel in your access points settings

//define mqtt topics
const char* mqtt_topic = "/home/picoclick/#";
const char* mqtt_topic_state = "/home/picoclick/state";
const char* mqtt_topic_battery = "/home/picoclick/battery";

const char* mqtt_client = "picoclick";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

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


int getBatteryVoltage() {
  float factor = 5; // (R1+R2)/R2, +/- X
  return (float(analogRead(0)) / 1023.0 * factor) * 1000.0; //returns mV
}

void ChargingStatus() {
  if (digitalRead(status_mcp) == 0) {
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV((hue + i * 85) % 255, 250, 100);
    FastLED.show();
    delay(50);
    hue = (hue + 1) % 255;
  }
  else {
    //digitalWrite(vdiv_gnd, HIGH);
    CRGB c = CRGB(100, 100, 100);
    if (getBatteryVoltage() > 4100) c = CRGB(0, 100, 0);
    else c = CRGB(100, 50, 0);

    for (int i = 0; i < NUM_LEDS; i++) leds[i] = c;
    FastLED.show();

    delay(3000);
    digitalWrite(latch, LOW);
    delay(1000);
  }
}

void ConnectionCheck(){
  int v = getBatteryVoltage();
    if (v < LIPO_MINIMAL) {
      for (int b = 100; b > 10; b--) {
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(b, 0, 0);
        FastLED.show();
        delay(10);
      }

      delay(200);
      digitalWrite(latch, LOW);
    }

    if (millis() > 6000) {
      for (int x = 0; x < 10; x++) {
        for (int i = 0; i < NUM_LEDS; i++){
          leds[i] = CRGB(100, 0, 0);
        } 
        FastLED.show();
        delay(50);
        
        delay(50);
      }
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(100, 0, 0);
      FastLED.show();
      delay(200);

      digitalWrite(latch, LOW);
    }
}

void setup() {
  pinMode(latch, OUTPUT);
  digitalWrite(latch, HIGH);

  pinMode(status_mcp, INPUT_PULLUP); //read charging status

  pinMode(button, INPUT);
  state = digitalRead(button);

  FastLED.addLeds<WS2812, 14, GRB>(leds, 3).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(200);

  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 0);
  FastLED.show();

  if (digitalRead(status_mcp) == 0) {
    //usb cable plugged in
    battery_charging = true;

    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(100);
  }
  else {
    //send mqtt message
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password, wifi_channel, bssid, true);
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 150);
    FastLED.show();

    int c_led = 0;
    while ((WiFi.status() != WL_CONNECTED)) {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, sin(3.141*float(c_led+i)/180.0)*150);
      FastLED.show();
      delay(10);

      ConnectionCheck();
      c_led++;
    }

    client.setServer(mqtt_server, 1883);

    while (!client.connected()) {
      if (client.connect(mqtt_client)) {
        client.subscribe(mqtt_topic_state);

        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 100, 0);
        FastLED.show();

        client.publish(mqtt_topic_state, "1");

        for (int b = 100; b > 10; b--) {
          for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, b, 0);
          FastLED.show();
          delay(10);
        }

        delay(100);
        while (digitalRead(button) == 1) {
          stay_on = true;
          for (int x = 0; x < 5; x++) {
            leds[0] = CRGB(0, 0, (x % 2) * 100);
            leds[1] = CRGB(0, 0, ((x + 1) % 2) * 100);
            FastLED.show();
            delay(50);
          }
        }

        if (!stay_on) digitalWrite(latch, LOW);
        delay(200);

        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 100);
        FastLED.show();
      }
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV((++hue + i * 127) % 255, 250, 150);
      FastLED.show();
      //delay(10);

      ConnectionCheck();
    }
    
  }
}

void loop() {
   if (!stay_on) {
    ChargingStatus();
  }
  else {
    if (digitalRead(button) == 1) {
      delay(100);
      long t = millis();
      while (digitalRead(button) == 1) {

        if (millis() > t + 1000) {

          for (int b = 100; b > 10; b--) {
            for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(b, 0, 0);
            FastLED.show();
            delay(10);
          }

          while (digitalRead(button) == 1) delay(20);

          delay(200);
          digitalWrite(latch, LOW);
          delay(100);
        }
      }
      delay(10);
    }
  }
}
