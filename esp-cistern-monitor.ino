#include "DHT.h"
#include "ESP8266WiFi.h"
#include "InfluxDb.h"
#include "NewPing.h"

#define DEBUG             false 

// Influx Config
#define INFLUXDB_HOST     "influxdb.hubspace.de"
#define INFLUXDB_PORT     "8086"
#define INFLUXDB_DATABASE "lasse"
#define INFLUXDB_USER     "lasse"
#define INFLUXDB_PASS     "Vuson:Ibihi284"

// WIFI Config
#define WIFISSID          "maeusekaefig"
#define WIFIPSK           "386L8P99L668F8HE"

// HW Config
#define TRIGGER D1
#define ECHO D2
#define MAX_DIST 300
#define DHTPIN D4
#define DHTTYPE DHT22

// global declarations
DHT dht22(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER, ECHO, MAX_DIST);
InfluxData entry(String type, String sensor, float value) {
  InfluxData row(type);
  row.addTag("chipid", String(ESP.getChipId()));
  row.addTag("device", "esp8266");
  row.addTag("sensor", sensor);
  row.addValue("value", value);
  return row;
}

void setup() {
  // INITIALIZE
  Serial.begin(115200);  
  if (DEBUG) { Serial.println("running..."); }
  
  // begin wifi and wait until it is connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFISSID, WIFIPSK);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  if (DEBUG) { Serial.print("WiFi connected. IP: "); Serial.print(WiFi.localIP()); Serial.print(" DNS: "); Serial.println(WiFi.dnsIP()); }

  // begin sensors
  dht22.begin();
  
  // begin influxdb connect & upload
  Influxdb influx(INFLUXDB_HOST);
  influx.setDbAuth(INFLUXDB_DATABASE, INFLUXDB_USER, INFLUXDB_PASS);
  InfluxData level = entry("level", "ultrasonic", takeLevel());
  influx.prepare(level);
  InfluxData humidity = entry("humidity", "dht22", takeHumidity());
  influx.prepare(humidity);
  InfluxData temp = entry("temp", "dht22", takeTemp());
  influx.prepare(temp);
  InfluxData voltage = entry("voltage", "batteryshield", takeVoltage());
  influx.prepare(voltage);
  InfluxData wifistrength = entry("wifistrength", "wifi", takeWifiStrength());
  influx.prepare(wifistrength);
  influx.write();
  
  // deep sleep to save power
  if (DEBUG) { Serial.println("don't sleep for 30m"); }
  if (DEBUG == false) {
    ESP.deepSleep(18e8);
  }
  delay(100);
}

void loop() {
  // nothing to loop while running in production
  if (DEBUG) {
    Serial.print("Level: ");
    Serial.println(takeLevel());
    Serial.print("Temperature: ");
    Serial.print(takeTemp());
    Serial.print(" | Humidity: ");
    Serial.println(takeHumidity());
    Serial.print("WiFi Strength: ");
    Serial.print(takeWifiStrength());
    Serial.print(" | Voltage: ");
    Serial.println(takeVoltage());
    Serial.println("--------------------");
    delay(10000);
  }
}

// measure dht22 humidity
float takeHumidity() {
  float result = dht22.readHumidity();
  if (isnan(result)) {
    return -1.0;
  }
  return result;
}

// measure ultrasonic distance
float takeLevel() {
  float result = float(sonar.ping_cm());
  if (result == 0) {
    return -1.0;
  }
  return result;
}

// measure dht22 temperature
float takeTemp() {
  float result = dht22.readTemperature();
  if (isnan(result)) {
    return -99.0;
  }
  return result;
}

// measure battery voltage
float takeVoltage() {
  unsigned int raw=0;
  float voltage=0.0;
  pinMode(A0, INPUT);
  raw = analogRead(A0);
  voltage=raw/1023.0;
  voltage=voltage*3.7;
  if (isnan(voltage) || voltage == 0) {
    return -1.0;
  }
  return voltage;
}

// measure wifi strength
float takeWifiStrength() {
  return float(WiFi.RSSI());
}
