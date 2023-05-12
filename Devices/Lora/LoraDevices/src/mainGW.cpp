#include <Arduino.h>
#include "config.h"

#ifndef NODE

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "HttpEth.h"
#include "string.h"
#include "Utils.h"
#include "OLED.h"
#include <time.h>

OLED display;

boolean runEvery(unsigned long interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Execution started");
    display.begin();
    display.gateway();
}

int num = 0;

void loop() {
  if (runEvery(10000)) { 
      char numero[2] = "0";
      sprintf(numero, "%d", num);
      display.gateway_parpadeo(numero);
      ++num;
  }
}

#endif