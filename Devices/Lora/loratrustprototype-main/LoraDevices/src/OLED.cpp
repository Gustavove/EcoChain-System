#include "OLED.h"

OLED::OLED() {

  disp = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

}

void OLED::begin() {
  //reset OLED display via software for ESP32LORA
  // #ifdef ESP32LORA 
  if (OLED_RST != -1) {
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(20);
    digitalWrite(OLED_RST, HIGH);
  }
  // #endif
// pins for ESP32LORA

   //initialize OLED
  Wire.begin((int) OLED_SDA, (int) OLED_SCL);
  if (!disp.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void OLED::scrollText(String data) {
  disp.clearDisplay();

#ifdef NODE
  data = "NODE WALLET @:\n\n0x" + data;
#else
  data = "GW WALLET @:\n\n0x" + data;
#endif

  disp.setTextSize(1); // Draw 2X-scale text
  disp.setTextColor(SSD1306_WHITE);
  disp.setCursor(0, 0);
  disp.println(data);
  disp.display();      // Show initial text

  // Scroll in various directions, pausing in-between:
  /*disp.startscrollleft(0x00, 0x0F);
  delay(2000);*/
}

