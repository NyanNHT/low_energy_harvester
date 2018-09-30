/*
   ESP8266 Chipset Core Boards - http://arduino.esp8266.com/stable/package_esp8266com_index.json
   DHT11 Library for ESP8266 Chipset - https://github.com/jdattilo/DHT11LIB
   Blynk Library - https://github.com/blynkkk/blynk-library/releases/download/v0.5.3/Blynk_Release_v0.5.3.zip
   OneWire Library - https://github.com/PaulStoffregen/OneWire
   DS18B20 Library - https://github.com/milesburton/Arduino-Temperature-Control-Library
   NodeMCU Digital & GPIO Pin Layout - https://alselectro.files.wordpress.com/2017/10/nodemcu_pins1.jpg
*/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define BLYNK_PRINT Serial
#define ONE_WIRE_BUS D4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

BlynkTimer timer;

char auth[] = "80fbaca812124696b9d355e599af4d70";
char ssid[] = "ukyizin";
char pass[] = "khwarnyo";
byte leds[6] = {D1,D2,D5,D6,D7,D8};

boolean output = false;
float tempDiffThres = 0.0;
float maxHotTemp = 0.0;

BLYNK_WRITE(V4) {
  tempDiffThres = param.asFloat();
}

BLYNK_WRITE(V5) {
  maxHotTemp = param.asFloat();
}

void flipLeds(bool state, byte led[]) {
  for (byte i = 0; i < 6; i++) {
    digitalWrite(led[i], state);
  }
}

void timerEvent() {
  Serial.println("Timer Event");
  sensors.requestTemperatures();
  float cold = sensors.getTempCByIndex(0);
  float hot = sensors.getTempCByIndex(1);
  float tempDiff = hot - cold;
  bool buffOutput = output;
  if (maxHotTemp > hot) {
    if (tempDiffThres < tempDiff) {
      output = true;
    } else {
      output = false;
    }
  } else {
    output = false;
  }
  if(buffOutput&&(!output)){
    Blynk.notify("Power goes down!");
  }
  flipLeds(output, leds);
  Blynk.virtualWrite(V1, cold);
  Blynk.virtualWrite(V2, hot);
  Blynk.virtualWrite(V3, (output * 1023));
  Serial.print("Cold -> ");
  Serial.println(cold);
  Serial.print("Hot -> ");
  Serial.println(hot);
  Serial.print("Output -> ");
  Serial.println(output);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Started");
  delay(1000);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  delay(1000);
  flipLeds(output, leds);
  delay(1000);
  Blynk.begin(auth, ssid, pass);
  Serial.println("Sensor Begin");
  sensors.begin();
  timer.setInterval(2000L, timerEvent);
  Blynk.syncVirtual(V4, V5);
}

void loop()
{
  delay(500);
  Blynk.run();
  timer.run();
}
