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
 * 02.03.2021 by @maker.moekoe (Moritz König)
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <PubSubClient.h>

#define ISR_PREFIX ICACHE_RAM_ATTR

const char* ssid = "XXX";
const char* password = "XXX";
const char* mqtt_server = "192.168.2.103";
const uint8_t bssid[6] = {0x80, 0x71, 0x7A, 0x3F, 0x8B, 0x63}; // anpassen
const int wifi_channel = 9;

const char* mqtt_topic = "/home/pico5/#";
const char* mqtt_topic_state = "/home/pico5/state";
const char* mqtt_client = "pico5";

WiFiClient espClient;
PubSubClient client(espClient);

#define LIPO_MINIMAL 3200 //mV
#define CONNECTION_TIMEOUT 6000 //ms

#define NUM_LEDS 3
CRGB leds[NUM_LEDS];

const int button = 12;
const int latch = 13;
const int status_mcp = 4;

volatile int interruptCounter = 1;
volatile unsigned long t_press = millis();
volatile bool btn_state = true;
bool longpress = false;

volatile boolean first_press = true;
volatile unsigned long last_interrupt_time = 0;

ISR_PREFIX void handleInterrupt() {
  if (millis() - last_interrupt_time > 70 || first_press){
    interruptCounter++;
    first_press = false;
    btn_state = digitalRead(button);
  }
  last_interrupt_time = millis();
  t_press = millis();
}

int getBatteryVoltage() {
  float factor = 5.0; // (R1+R2)/R2, +/- X
  return (float(analogRead(0)) / 1023.0 * factor) * 1000.0; //returns mV
}

void ConnectionCheck(){
  int v = getBatteryVoltage();
  if (v < LIPO_MINIMAL || millis() > CONNECTION_TIMEOUT) {
    for (int b = 100; b > 10; b--) {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(b, 0, 0);
      FastLED.show();
      delay(10);
    }
    delay(200);
    digitalWrite(latch, LOW);
  }
}

void setup() {
  pinMode(latch, OUTPUT);
  digitalWrite(latch, HIGH);

  pinMode(button, INPUT);
  attachInterrupt(digitalPinToInterrupt(button), handleInterrupt, CHANGE);
  pinMode(status_mcp, INPUT_PULLUP);

  FastLED.addLeds<WS2812, 14, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  delay(1);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, wifi_channel, bssid, true);
  delay(1);
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0, 0, 150);
  FastLED.show();

  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    ConnectionCheck();
  }

  client.setServer(mqtt_server, 1883);

  while (!client.connected()) {
    if (client.connect(mqtt_client)) {
      client.subscribe(mqtt_topic_state);
    }
    ConnectionCheck();
  }
}

void loop() {
  if(digitalRead(button) && interruptCounter >= 0 && btn_state && millis() > t_press + 300){
    for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(150, 0, 0);
    FastLED.show();
    interruptCounter = -interruptCounter;
  }

  if(millis() > t_press + 500) {
    int n_button_press = (interruptCounter > 0) ? interruptCounter / 2 : (interruptCounter - 1) / 2;
    char b[10];
    itoa(n_button_press, b, 10);
    client.publish(mqtt_topic_state, b);

    delay(10);
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();

    if(n_button_press < 0) longpress = true;

    for(int n=0; n<abs(n_button_press*2)-1; n++){
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(longpress*150*(!(n%2)), (!longpress)*150*(!(n%2)), 0);
      FastLED.show();
      while(digitalRead(button)) delay(1);
      delay(200);
    }

    for (int b = 150; b > 10; b--) {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(longpress*b, !longpress*b, 0);
      FastLED.show();
      delay(2);
    }

    digitalWrite(latch,0);
    delay(200);
  }
}
