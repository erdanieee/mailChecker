#include <Arduino.h>
#include <main.h>
#include <private.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   //https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

#define pinSwithNewMail 13      // interrupt TODO: check
#define pinSwithOpen    14      // interrupt TODO: check
#define pinOTA          4       // IO        TODO: check


#ifndef PRIVATE
  #define BOTtoken "XXXXXXXXXXXXX"
  const char* ssid = "XXXXXXXXXXXXX";
  const char* password = "XXXXXXXXXXXXX";
  const char* chat_id = "XXXXXXXXXXXXX";
#endif



bool isNewEmail, isOpened, isOTAEnabled;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

unsigned long Bot_lasttime;

extern "C" {
#include "user_interface.h"
}


void newEmailISR(){
  isNewEmail = true;
}

void openISR(){
  isOpened = true;
}



void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(pinSwithNewMail,  INPUT_PULLUP);
  pinMode(pinSwithOpen,     INPUT_PULLUP);
  pinMode(pinOTA,           INPUT_PULLUP);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  wifi_set_sleep_type(LIGHT_SLEEP_T);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  attachInterrupt(pinSwithNewMail,  newEmailISR, FALLING);
  attachInterrupt(pinSwithOpen,     openISR, FALLING);
}




void loop() {
  if (isNewEmail){
    bot.sendMessage(chat_id, "\u0001F4E8 Nuevo correo \u2709\ufe0f\n", "");
    isNewEmail = false;
  }

  if (isOpened){
    bot.sendMessage(chat_id, "\u0001F4EC Buzón abierto \u2709\ufe0f", "");
    isOpened = false;
  }

  delay(10000);
}
