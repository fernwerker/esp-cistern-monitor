#include "DHT.h"
#include "ESP8266WiFi.h"
#include "NewPing.h"

// General Config
#define DEBUG true

// WIFI Config
#define STASSID "your-ssid"
#define STAPSK  "your-password"

// HW Config
#define TRIGGER D1
#define ECHO D2
#define MAX_DIST 300
#define DHTPIN D4
#define DHTTYPE DHT22

// declare global variables
bool debug            = DEBUG;

void setup() {
  initialize();
  int level = takeLevel();
  float humid = takeHumid();
  float temp = takeTemp();
  ESP.deepSleep(3.6e9);
}

void loop() {
  // nothing to loop while running
}

void initialize() {
  // write wifi variables
  const char* ssid      = STASSID;
  const char* password  = STAPSK;

  // init chip: serial, wifi-settings, shields
  if (debug) {
    Serial.begin(115200);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

float takeHumid() {
  // measure dht22 humidity
  return 100.0;
}

int takeLevel() {
  // measure ultrasonic distance
  return 42;
}

float takeTemp() {
  // measure dht22 temperature
  return 42.0;
}
