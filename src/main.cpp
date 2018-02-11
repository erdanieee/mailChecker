#include <Arduino.h>
#include <main.h>
#include <private.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   //https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

#ifndef PRIVATE
  #define BOTtoken        "XXXXXXXXXXXXX"
  const char* ssid      = "XXXXXXXXXXXXX";
  const char* password  = "XXXXXXXXXXXXX";
  const char* chat_id   = "XXXXXXXXXXXXX";
#endif


#define pinSwithNewMail 13      // interrupt TODO: check
#define pinSwithOpen    14      // interrupt TODO: check
#define pinOTA          4       // IO        TODO: check

#define MAX_TIME_EMAIL_BLOCKED    5000
#define MAX_TIME_DOOR_OPENED     10000


bool isNewEmail, isDoorOpened, isMailInBlocked, isDoorLeavedOpen;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

unsigned long mail_lastTime, open_lastTime;


extern "C" {
#include "user_interface.h"
}


void newEmailISR(){
  isNewEmail = true;
}

void openISR(){
  isDoorOpened = true;
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  attachInterrupt(pinSwithNewMail,  newEmailISR, FALLING);
  attachInterrupt(pinSwithOpen,     openISR, FALLING);
  wifi_set_sleep_type(LIGHT_SLEEP_T);
}




void loop() {
  if (isNewEmail){
    mail_lastTime = millis();
    bot.sendMessage(chat_id, "\u0001F4E8 Nuevo correo \u2709\ufe0f\n", "");
    isNewEmail = false;
    isMailInBlocked = false;
    delay(100);
  }

  if (isDoorOpened){
    open_lastTime = millis();
    bot.sendMessage(chat_id, "\u0001F4EC Buzón abierto \u2709\ufe0f", "");
    isDoorOpened = false;
    isDoorLeavedOpen = false;
    delay(100);
  }

  if ( digitalRead(pinSwithNewMail) == LOW &&
          !isMailInBlocked &&
          (millis() - mail_lastTime) > MAX_TIME_EMAIL_BLOCKED){
      bot.sendMessage(chat_id, "Hay algo bloqueando la entrada del buzón... ¿publicidad?", "");
      isMailInBlocked = true;
  }

  if ( digitalRead(pinSwithOpen) == LOW &&
          !isDoorLeavedOpen &&
          (millis() - open_lastTime) > MAX_TIME_DOOR_OPENED){
      bot.sendMessage(chat_id, "¡¡¡ATENCIÓN!! ¡¡¡El buzón se ha quedado abierto!!!", "");
      isDoorLeavedOpen = true;
  }

  delay(5000);
}
