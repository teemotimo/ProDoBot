#include <MD_MAX72xx.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "esp_eap_client.h"

// ============= WIFI - CHANGE THESE =============
#define WIFI_SSID "SFUNET-SECURE"
#define EAP_IDENTITY "tka132@sfu.ca"
#define EAP_PASSWORD "fr3nchT0ast@417111"

// ============= MQTT - DON'T CHANGE =============
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// ============= MATRIX SETUP =============
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define DATA_PIN 13
#define CLK_PIN 14
#define CS_PIN 15

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ============= LCD SETUP =============
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============= MQTT =============
WiFiClient espClient;
PubSubClient client(espClient);

String currentEmotion = "idle"; //CHANGE BACK TO IDLE DUMBASSSSSSSSSS

// ============= WIFI CONNECT =============
void setwifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  esp_eap_client_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_eap_client_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_eap_client_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_enterprise_enable();

  WiFi.begin(WIFI_SSID);
  
  // Timeout after 10 seconds
  int timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
  } else {
    Serial.println("\nWiFi FAILED - continuing anyway");
  }
}

// ============= MQTT CALLBACK =============
void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);
  const char* emotion = doc["emotion"] | "moodChill";
  currentEmotion = String(emotion);
  Serial.print("Received: ");
  Serial.println(currentEmotion);
}

// ============= MQTT RECONNECT =============
void reconnect() {
  if (!client.connected()) {
    String clientId = "ESP32-Display-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("room/display");
      Serial.println("MQTT connected!");
    }
  }
}

// ============= HELPER: SET PIXEL =============
void setPixel(int x, int y, bool state) {
  matrix.setPoint(x, y, state);
}

// ============= SETUP =============
void setup() {
  Serial.begin(115200);

  matrix.begin();
  matrix.clear();
  matrix.control(MD_MAX72XX::INTENSITY, 8);

  lcd.init();
  lcd.backlight();

  setwifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Ready!");
}

// ============= LOOP =============
void loop() {
  reconnect();
  client.loop();
  
  if (currentEmotion == "idle") {
    idle();
  }
  if (currentEmotion == "moodChill") {
    moodChill();
  }
  if (currentEmotion == "moodWarm") {
    moodWarm();
  }
  if (currentEmotion == "moodFocus") {
    moodFocus();
  }
}

// ============= MOOD: CHILL =============
void moodChill() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mood: Chill");

  // Snowflake animation frame 1
  matrix.clear();
  setPixel(3, 4, true);
  delay(500);

  // Snowflake animation frame 2
  matrix.clear();
  setPixel(3, 4, true);
  setPixel(3, 5, true);
  setPixel(3, 3, true);
  setPixel(2, 4, true);
  setPixel(4, 4, true);
  delay(500);

  // Snowflake animation frame 3
  matrix.clear();
  setPixel(3,5, true);
setPixel(3,3, true);
setPixel(2,4, true);
setPixel(4,4, true);

setPixel(2,6, true);
setPixel(4,6, true);
setPixel(2,2, true);
setPixel(4,2, true);
setPixel(5,3, true);
setPixel(5,5, true);
setPixel(1,3, true);
setPixel(1,5, true);

  delay(500);

  matrix.clear();
  setPixel(0,3,true);
setPixel(0,4,true);

// Row 1
setPixel(1,1,true);
setPixel(1,3,true);
setPixel(1,4,true);
setPixel(1,6,true);

// Row 2
setPixel(2,2,true);
setPixel(2,5,true);

// Row 3
setPixel(3,0,true);
setPixel(3,1,true);
setPixel(3,3,true);
setPixel(3,4,true);  // center anchor
setPixel(3,6,true);
setPixel(3,7,true);

// Row 4
setPixel(4,0,true);
setPixel(4,1,true);
setPixel(4,3,true);
setPixel(4,4,true);
setPixel(4,6,true);
setPixel(4,7,true);

// Row 5
setPixel(5,2,true);
setPixel(5,5,true);

// Row 6
setPixel(6,1,true);
setPixel(6,3,true);
setPixel(6,4,true);
setPixel(6,6,true);

// Row 7
setPixel(7,3,true);
setPixel(7,4,true);

delay(2000);


  //COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND

//COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND

//COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND

//COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND

//COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND

//COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND

//COOL FACE 
  matrix.clear();
setPixel(1,6, true);
setPixel(1,5, true);
setPixel(1,4, true);
setPixel(2,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(5,5, true);
setPixel(5,4, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(1,1, true);
setPixel(3,1, true);
setPixel(5,1, true);
setPixel(7,1, true);

delay(1000);

matrix.clear();
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(2,5, true);
setPixel(2,4, true);
setPixel(6,6, true);
setPixel(6,5, true);
setPixel(6,4, true);
setPixel(5,6, true);

setPixel(0,0, true);
setPixel(1,0, true);
setPixel(2,0, true);
setPixel(3,0, true);
setPixel(4,0, true);
setPixel(5,0, true);
setPixel(6,0, true);
setPixel(7,0, true);

setPixel(1,2, true);
setPixel(2,2, true);
setPixel(3,2, true);
setPixel(4,2, true);
setPixel(5,2, true);
setPixel(6,2, true);
setPixel(7,2, true);
setPixel(0,2, true);

setPixel(0,1, true);
setPixel(6,1, true);
setPixel(2,1, true);
setPixel(4,1, true);

delay(1000);
//LOOPEND


}

// ============= MOOD: WARM =============
void moodWarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mood: Warm");

  // Sun animation frame 1
  matrix.clear();
  setPixel(2,3,true); setPixel(2,4,true);
setPixel(3,2,true); setPixel(3,5,true);
setPixel(4,2,true); setPixel(4,5,true);
setPixel(5,3,true); setPixel(5,4,true);

  delay(500);

  // Sun animation frame 2 - add rays
  matrix.clear();
  setPixel(1, 3, true);
  setPixel(1, 4, true);
  setPixel(6, 3, true);
  setPixel(6, 4, true);
  setPixel(3, 1, true);
  setPixel(4, 1, true);
  setPixel(3, 6, true);
  setPixel(4, 6, true);
  delay(500);

  // Sun animation frame 3 - clear rays
  matrix.clear();
  setPixel(3, 3, true);
  setPixel(3, 4, true);
  setPixel(4, 3, true);
  setPixel(4, 4, true);
  delay(4000);

//COOL FACE 
  matrix.clear();
  setPixel(0,9, true);
setPixel(1,9, true);
setPixel(2,9, true);
setPixel(3,9, true);
setPixel(4,9, true);
setPixel(5,9, true);
setPixel(6,9, true);
setPixel(7,9, true);
setPixel(0,8, true);
setPixel(1,8, true);
setPixel(2,8, true);
setPixel(7,8, true);
setPixel(6,8, true);
setPixel(5,8, true);
setPixel(6,7, true);
setPixel(1,7, true);

setPixel(6,5, true);
setPixel(5,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

delay(1000);

//COOL FACE 
  matrix.clear();
  setPixel(0,8, true);
setPixel(1,8, true);
setPixel(2,8, true);
setPixel(3,8, true);
setPixel(4,8, true);
setPixel(5,8, true);
setPixel(6,8, true);
setPixel(7,8, true);
setPixel(0,7, true);
setPixel(1,7, true);
setPixel(2,7, true);
setPixel(7,7, true);
setPixel(6,7, true);
setPixel(5,7, true);
setPixel(6,6, true);
setPixel(1,6, true);

setPixel(6,5, true);
setPixel(5,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

delay(1000);

//COOL FACE 
  matrix.clear();
  setPixel(0,7, true);
setPixel(1,7, true);
setPixel(2,7, true);
setPixel(3,7, true);
setPixel(4,7, true);
setPixel(5,7, true);
setPixel(6,7, true);
setPixel(7,7, true);
setPixel(0,6, true);
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(7,6, true);
setPixel(6,6, true);
setPixel(5,6, true);
setPixel(6,5, true);
setPixel(1,5, true);

setPixel(6,5, true);
setPixel(5,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

delay(1000);

//COOL FACE 
  matrix.clear();
  setPixel(0,6, true);
setPixel(1,6, true);
setPixel(2,6, true);
setPixel(3,6, true);
setPixel(4,6, true);
setPixel(5,6, true);
setPixel(6,6, true);
setPixel(7,6, true);
setPixel(0,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(7,5, true);
setPixel(6,5, true);
setPixel(5,5, true);
setPixel(6,4, true);
setPixel(1,4, true);

setPixel(6,5, true);
setPixel(5,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

delay(1000);

//COOL FACE 
  matrix.clear();
  setPixel(0,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(3,5, true);
setPixel(4,5, true);
setPixel(5,5, true);
setPixel(6,5, true);
setPixel(7,5, true);
setPixel(0,4, true);
setPixel(1,4, true);
setPixel(2,4, true);
setPixel(7,4, true);
setPixel(6,4, true);
setPixel(5,4, true);
setPixel(6,3, true);
setPixel(1,3, true);

setPixel(6,5, true);
setPixel(5,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

delay(1000);

  //COOL FACE 
  matrix.clear();
  setPixel(0,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(3,5, true);
setPixel(4,5, true);
setPixel(5,5, true);
setPixel(6,5, true);
setPixel(7,5, true);
setPixel(0,4, true);
setPixel(1,4, true);
setPixel(2,4, true);
setPixel(7,4, true);
setPixel(6,4, true);
setPixel(5,4, true);
setPixel(6,3, true);
setPixel(1,3, true);

setPixel(6,5, true);
setPixel(5,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

delay(1000);

matrix.clear();
  setPixel(0,5, true);
setPixel(1,5, true);
setPixel(2,5, true);
setPixel(3,5, true);
setPixel(4,5, true);
setPixel(5,5, true);
setPixel(6,5, true);
setPixel(7,5, true);
setPixel(0,4, true);
setPixel(1,4, true);
setPixel(2,4, true);
setPixel(7,4, true);
setPixel(6,4, true);
setPixel(5,4, true);
setPixel(6,3, true);
setPixel(1,3, true);

setPixel(2,1, true);
setPixel(3,1, true);
setPixel(4,2, true);

}

// ============= MOOD: FOCUS =============
void moodFocus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mood: Focus");

  // Target/crosshair animation
  matrix.clear();

  // Center dot
  setPixel(3, 3, true);
  setPixel(3, 4, true);
  setPixel(4, 3, true);
  setPixel(4, 4, true);
  delay(300);

  // Add crosshair lines
  setPixel(0, 3, true);
  setPixel(0, 4, true);
  setPixel(7, 3, true);
  setPixel(7, 4, true);
  setPixel(3, 0, true);
  setPixel(4, 0, true);
  setPixel(3, 7, true);
  setPixel(4, 7, true);
  delay(500);

  // Blink center
  setPixel(3, 3, false);
  setPixel(3, 4, false);
  setPixel(4, 3, false);
  setPixel(4, 4, false);
  delay(200);

  setPixel(3, 3, true);
  setPixel(3, 4, true);
  setPixel(4, 3, true);
  setPixel(4, 4, true);
  delay(300);

}
void idle() {
  setPixel(0,4, true);
setPixel(1,5, true);
setPixel(2,4, true);

setPixel(5,4, true);
setPixel(6,5, true);
setPixel(7,4, true);

setPixel(2,2, true);
setPixel(3,1, true);
setPixel(4,1, true);
setPixel(5,2, true);

lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello! How can I ");
  lcd.setCursor(0,1);
  lcd.print("help you today?");

delay(1000);

matrix.clear();

setPixel(0,5, true);
setPixel(1,6, true);
setPixel(2,5, true);

setPixel(5,5, true);
setPixel(6,6, true);
setPixel(7,5, true);

setPixel(2,2, true);
setPixel(3,1, true);
setPixel(4,1, true);
setPixel(5,2, true);

delay(1000);

matrix.clear();



}

// ============= LOADING ANIMATION =============
void loading() {
  for (int i = 0; i <= 7; i++) {
    for (int j = 0; j <= 7; j++) {
      setPixel(i, j, true);
    }
    delay(50);
    for (int j = 0; j <= 7; j++) {
      setPixel(i, j, false);
    }
  }
}
