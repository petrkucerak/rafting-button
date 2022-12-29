#include "main.h"
#include "ESP-NOW/esp-now.h"
#include "board.h"
#include "callback.h"
#include "utils.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// variables
uint16_t color;

void broadcast(const String &message)
{
   // Broadcast a message to every device in range
   uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
   esp_now_peer_info_t peerInfo = {};
   memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
   if (!esp_now_is_peer_exist(broadcastAddress)) {
      esp_now_add_peer(&peerInfo);
   }
   // Send message
   esp_err_t result = esp_now_send(
       broadcastAddress, (const uint8_t *)message.c_str(), message.length());

   // Print results to serial monitor
   if (result == ESP_OK) {
      Serial.println("Broadcast message success");
   } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      Serial.println("ESP-NOW not Init.");
   } else if (result == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
   } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      Serial.println("Internal Error");
   } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
   } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      Serial.println("Peer not found.");
   } else {
      Serial.println("Unknown error");
   }
}

void esp_now_echo()
{
   USBSerial.printf("Start ECHO program\n");

   if (!esp_now_handler.isEmpty) {
      // init peer
      esp_now_peer_info_t peer_info;
      peer_info.channel = 0;
      peer_info.encrypt = false;
      memcpy(peer_info.peer_addr, esp_now_handler.sender_mac_addr, 6);
      delay(100);
      if (!esp_now_is_peer_exist(esp_now_handler.sender_mac_addr)) {

         uint8_t status = esp_now_add_peer(&peer_info);
         USBSerial.printf("The output is %d\n", status);

         if (status == ESP_ERR_ESPNOW_NOT_INIT) {
            USBSerial.println("ERROR: ESPNOW is not initialized");
            delay(2000);
            ESP.restart();
         } else if (status == ESP_ERR_ESPNOW_ARG) {
            USBSerial.println("ERROR:  invalid argument");
            delay(2000);
            ESP.restart();
         } else if (status == ESP_ERR_ESPNOW_NOT_FOUND) {
            USBSerial.println("ERROR: peer is not found");
            delay(2000);
            ESP.restart();
         }
      }
      // sent a message
      esp_err_t res = esp_now_send(esp_now_handler.sender_mac_addr,
                                   (uint8_t *)&esp_now_handler.data,
                                   esp_now_handler.data_len);

      // handler an error
      if (res == ESP_OK) {
         USBSerial.printf("Succes sent a message!\n");
      } else if (res == ESP_ERR_ESPNOW_NOT_INIT) {
         USBSerial.println("ESP-NOW not Init.");
      } else if (res == ESP_ERR_ESPNOW_ARG) {
         USBSerial.println("Invalid Argument");
      } else if (res == ESP_ERR_ESPNOW_INTERNAL) {
         USBSerial.println("Internal Error");
      } else if (res == ESP_ERR_ESPNOW_NO_MEM) {
         USBSerial.println("ESP_ERR_ESPNOW_NO_MEM");
      } else if (res == ESP_ERR_ESPNOW_NOT_FOUND) {
         USBSerial.println("Peer not found.");
      } else {
         USBSerial.println("Unknown error");
      }

      esp_now_handler.isEmpty = TRUE;
   }
}

void setup()
{

   all_pins_init();

   HWSerial.begin(115200);
   HWSerial.setDebugOutput(true);

   USB.onEvent(usb_event_callback);
   USBSerial.onEvent(usb_event_callback);

   USBSerial.begin();
   USB.begin();

   // init_usb();
   mac_on_display();

   // config ESP NOW broadcast
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   if (esp_now_init() == ESP_OK) {
      USBSerial.printf("Initialization ESP NOW has been success!\n");
      esp_now_register_recv_cb(receive_callback);
      esp_now_register_send_cb(sent_callback);
      esp_now_handler.isEmpty = TRUE;
   } else {
      USBSerial.printf("ERROR: Can't initialize ESP NOW!\n");
      delay(3000);
      ESP.restart();
   }

   pinMode(LED_RED_BUILDIN, OUTPUT);
   digitalWrite(LED_RED_BUILDIN, HIGH);
   delay(100);
   digitalWrite(LED_RED_BUILDIN, LOW);
}

void loop()
{

   // lcd_dev.lcd_set_color(COLOR_WHITE);
   delay(2000);
   // esp_now_echo();
   red_blick(50);
   broadcast("HELLO!");
   // int64_t time = esp_timer_get_time();
   // USBSerial.printf("Time since start: %d\n", time);
}