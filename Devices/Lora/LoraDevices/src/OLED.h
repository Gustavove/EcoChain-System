#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"


class OLED {

    private:
        Adafruit_SSD1306 disp;

    public:
        OLED();
        void begin();
        void scrollText(String);
        void scrollText_nom(String);
        void parpadeo(String);
        void gateway();
        void gateway_parpadeo(String);
};

#endif