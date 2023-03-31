#ifndef config_H
#define config_H

#define NODE  //Define if using node or comment to compile gateway  if defined, then NODE, in not defined, then GW.

//Wifi node info
#define SERVER_IP "192.168.1.15"
#define SERVER_PORT 443
#define SC_ADDRESS "0xE36C2604F9701FFc6a04f8e6bE7368C3BC83329d"
#define CHAIN_ID 1337

//WIFI configurations
#define RUT_SSID "YOUR_SSID"
#define WIFIPWD "YOUR_PASSWORD"

//LoRa configuration
#define FREQUENCY  866E6 // LoRa Frequency
#define CSPIN LORA_CS         // LoRa radio chip select
#define RSTLORAPIN LORA_RST    // LoRa radio reset
#define IRQPIN LORA_IRQ         // change for your board; must be a hardware interrupt pin 

#define SCK LORA_SCK
#define MISO LORA_MISO
#define MOSI LORA_MOSI
#define SS LORA_CS
#define RST LORA_RST
#define DIO0 LORA_IRQ

//Data sending rate in ms
#define SENDINGTIME 8000

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define O_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#ifndef OLED_SDA
#define OLED_SDA SDA
#endif
#ifndef OLED_SCL
#define OLED_SCL SCL
#endif

#ifndef OLED_RST
#define OLED_RST -1
#endif


#endif
