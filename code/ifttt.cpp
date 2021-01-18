/**
 * mqtt example for the Picoclick v2.2
 * 
 * This sketch activates an IFTTT.
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
#include <FastLED.h>

const char* ssid = "XXX";
const char* password = "XXX";
const char* mqtt_server = "192.168.2.103";
const uint8_t bssid[6] = {0x80, 0x71, 0x7A, 0x3F, 0x8B, 0x63}; //put your access points bssid here
const int wifi_channel = 9; //set the channel in your access points settings

// IFTTT setup
//const char *host = "maker.ifttt.com";
const char *host = "100.24.23.248";
const char *privateKey = "XXXX";

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



void send_event(const char *event) {

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return;
  }

  // We now create a URI for the request
  String url = "https://maker.ifttt.com/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Read all the lines of the reply from server and print them to Serial,
  // the connection will close when the server has sent all the data.
  while (client.connected())
  {
    if (client.available())
    {
      String line = client.readStringUntil('\r');
    } else {
      // No data yet, wait a bit
      delay(50);
    };
  }

  client.stop();
  // Finished sending the message
}

int getBatteryVoltage() {
  float factor = 10.49; // (R1+R2)/R2, +/- X
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
    //connect & act
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

    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 100, 0);
    FastLED.show();

    send_event("picoclick_1");

    for (int b = 100; b > 10; b--) {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, b, 0);
      FastLED.show();
      delay(10);
    }

    digitalWrite(latch, LOW);
    delay(200);
    //never reaching this
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
