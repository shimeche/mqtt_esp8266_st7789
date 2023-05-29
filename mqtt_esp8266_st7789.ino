#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#include "credentials.h"

// ST7789 TFT module connections
#define TFT_DC    D1     // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST   D2     // TFT RST pin is connected to NodeMCU pin D2 (GPIO4)
#define TFT_CS    D8     // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)

#define BLK_PIN D6

// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

WiFiClient client;
PubSubClient mqttClient(client);

int targetValue = 0;
int currentValue = 0;

void callback(char* topic, byte* payload, unsigned int length)
{
    analogWrite(BLK_PIN, 100);

    payload[length] = '\0';
    char* payloadchar = (char*) payload;

    // if (isNumeric(payloadchar)) {
    //   int value = String(payloadchar).toInt();
    //   printString(String(value));
    // } else {
      printString(String(payloadchar));
    // }
    
    delay(5000);
    analogWrite(BLK_PIN, 0);
    tft.fillScreen(ST77XX_BLACK);
}

bool isNumeric(char* payload) {
    int value = atoi(payload);
    return value != 0 || strcmp(payload, "0") == 0;
}

void setup()
{
    Serial.begin(9600);

    tft.init(240, 240, SPI_MODE2);

    // pinMode(BLK_PIN, OUTPUT);
    
    // if the screen is flipped, remove this command
    tft.setRotation(2);
    delay(1000);

    printString("Hi~");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        printString(".");
    }

    printString("Connected to Wi-Fi");

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(callback);

    while (!client.connected()) {
        if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD )) {
            printString("Connected to MQTT broker");
        } else {
            delay(500);
            printString(".");
        }
    }

    mqttClient.subscribe(TOPIC);
}

void loop()
{
    mqttClient.loop();
}

void printString(String text) {
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(8);
  tft.setTextWrap(true);
  tft.println(text);
}