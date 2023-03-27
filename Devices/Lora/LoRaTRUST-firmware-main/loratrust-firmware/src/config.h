#pragma once

//If the device has a GPS module
#define GPS_ENABLED

//Wallet Configuration
#ifndef A0
#define A0 A0
#endif

//Sensors Configuration
#define STORED_SENSOR_DATA 10
    //- Temperature Configuration
#define TEMPERATURE_ENABLED
#define ONE_WIRE_BUS 2
#define TEMP_UPDATE_DELAY 60000 //ms
    //- Metadata Configuration
#define METADATA_ENABLED
#define METADATA_UPDATE_DELAY 60000 //ms

//Using LILYGO TTGO T-BEAM v1.1 
#define BOARD_LED   4
#define LED_ON      LOW
#define LED_OFF     HIGH

//LoRaChat Configuration
// #define LORACHAT_ENABLED
#define MAX_NAME_LENGTH 10
#define MAX_MESSAGE_LENGTH 100
#define MAX_PREVIOUS_MESSAGES 10

//WiFi Configuration
#define MAX_CONNECTION_TRY 15
#define SERVER_CONNECTION_TIMEOUT 1000 //ms
#define SERVER_PORT 8080
#define SERVER_URL "http://192.168.1.11:3000/save"

//Logging Configuration
// #define DISABLE_LOGGING