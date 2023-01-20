#include "main.h"
#include "ESP-NOW/esp-now.h"
#include "USB-C/USB-C.h"
#include "board.h"
#include "utils.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define LATENCY_ARR_SIZE 10000
#define ERROR_TIME 500

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
   if (!esp_now_handler.isEmpty) {
      // init peer
      esp_now_peer_info_t peer_info = {};
      memcpy(peer_info.peer_addr, esp_now_handler.sender_mac_addr, 6);

      if (!esp_now_is_peer_exist(peer_info.peer_addr)) {
         esp_err_t status = esp_now_add_peer(&peer_info);
         if (status != ESP_OK) {
            USBSerial.printf("ERROR: Can't create a peer connection!\n");
            // if (status == ESP_ERR_ESPNOW_NOT_INIT)
            //    USBSerial.printf("ESP_ERR_ESPNOW_NOT_INIT\n");
            // if (status == ESP_ERR_ESPNOW_ARG)
            //    USBSerial.printf("ESP_ERR_ESPNOW_ARG\n");
            // if (status == ESP_ERR_ESPNOW_FULL)
            //    USBSerial.printf("ESP_ERR_ESPNOW_FULL\n");
            // if (status == ESP_ERR_ESPNOW_NO_MEM)
            //    USBSerial.printf("ESP_ERR_ESPNOW_NO_MEM\n");
            // if (status == ESP_ERR_ESPNOW_EXIST)
            //    USBSerial.printf("ESP_ERR_ESPNOW_EXIST\n");
         }
      }

      // sent a message
      if (esp_now_send(esp_now_handler.sender_mac_addr,
                       (uint8_t *)&esp_now_handler.data,
                       esp_now_handler.data_len) != ESP_OK) {
         USBSerial.printf("ERROR: Can't sent a message!\n");
      }
      esp_now_handler.isEmpty = TRUE;
   }
}

void esp_now_test_latency(uint16_t message_count, uint8_t message_size,
                          uint8_t *mac_address)
{
   // Check parametrs
   if (message_size <= 0 || message_size > 250) {
      USBSerial.printf(
          "ERROR: The message size must be between 0 and 250 bytes!\n");
      delay(5000);
      return;
   }
   if (message_count <= 0) {
      USBSerial.printf("ERROR: The message count must be upper than 0!\n");
      delay(5000);
      return;
   }

   // Init the ESP NOW structure
   esp_now_peer_info_t peer_info = {};
   // If mac address target is NULL, send broadcast
   if (mac_address == NULL) {
      uint8_t broadcast_address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      memcpy(&peer_info.peer_addr, broadcast_address, 6);
   } else {
      memcpy(&peer_info.peer_addr, mac_address, 6);
   }
   if (!esp_now_is_peer_exist(peer_info.peer_addr)) {
      esp_now_add_peer(&peer_info);
   }

   // Create memory for message
   uint8_t data[message_size];
   // Create memory for results
   int64_t *latency = NULL;
   latency = (int64_t *)malloc(LATENCY_ARR_SIZE * sizeof(int64_t));
   if (latency == NULL) {
      USBSerial.printf("ERROR: Can't allocate memmory!\n");
      delay(5000);
      return;
   }

   // The bit transmition error
   int64_t ber = 0;

   int64_t start, end;
   esp_err_t status;

   for (uint64_t i = 0; i < LATENCY_ARR_SIZE; ++i) {
      latency[i] = 0;
   }

   // Clean the handler status
   esp_now_handler.isEmpty = TRUE;

   char mac_addr_string[18];
   format_mac_address(peer_info.peer_addr, mac_addr_string, 18);

   // Start a measurement
   USBSerial.printf("===================\n");
   USBSerial.printf("START A MEASUREMENT\n");
   USBSerial.printf("Payload: %d\nMeasurements count: %d\nTarget IP: %s\n",
                    message_size, message_count, mac_addr_string);
   uint16_t origin = message_count;
   while (message_count > 0) {

      progress_bar_on_display((origin - message_count) * 100 / origin);

      uint64_t error_time = ERROR_TIME;

      // set esp handler as an empty to prevent error by ignored message
      esp_now_handler.isEmpty = TRUE;

      // save start time
      start = esp_timer_get_time();

      // send a message
      status = esp_now_send(peer_info.peer_addr, data, message_size);

      // handler sending error
      if (status != ESP_OK) {
         USBSerial.printf("ERROR: Error ESP NOW code: %d\n", status);
         delay(5000);
         return;
      }

      // wait for echo message
      while (esp_now_handler.isEmpty && error_time != 0) {
         vTaskDelay(1);
         --error_time;
      }

      // save end time
      end = esp_timer_get_time();

      // set esp handler as an empty
      esp_now_handler.isEmpty = TRUE;

      if (!error_time) {
         // save error
         ++ber;
      } else {
         // save delay
         if (end - start < LATENCY_ARR_SIZE)
            ++latency[end - start];
      }
      --message_count;
   }

   remove_progress_bar_from_display();
   delay(1000);
   saving_data_on_display();
   delay(1000);

   USBSerial.printf("\ntime=[");
   for (int i = 0; i < LATENCY_ARR_SIZE; ++i) {
      if (latency[i] != 0)
         USBSerial.printf("%d,", i);
   }
   USBSerial.printf("]\nvalue=[");
   for (int i = 0; i < LATENCY_ARR_SIZE; ++i) {
      if (latency[i] != 0)
         USBSerial.printf("%lld,", latency[i]);
   }
   USBSerial.printf("]\n");
   USBSerial.printf("Ber: %lld\n", ber);

   free(latency);
   latency = NULL;
   USBSerial.printf("\n");

   remove_saving_data_from_display();
   delay(1000);
}

void setup()
{

   all_pins_init();

   pinMode(LED_RED_BUILDIN, OUTPUT);
   digitalWrite(LED_RED_BUILDIN, HIGH);

   HWSerial.begin(115200);
   HWSerial.setDebugOutput(true);

   USB.onEvent(usbc_dev.usb_event_callback);
   USBSerial.onEvent(usbc_dev.usb_event_callback);

   USBSerial.begin();
   USB.begin();

   mac_on_display();

   // config ESP NOW broadcast
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   if (esp_now_init() == ESP_OK) {
      USBSerial.printf("Initialization ESP NOW has been success!\n");
      esp_now_register_recv_cb(esp_now_dev.receive_callback);
      esp_now_register_send_cb(esp_now_dev.sent_callback);
      esp_now_handler.isEmpty = TRUE;
   } else {
      USBSerial.printf("ERROR: Can't initialize ESP NOW!\n");
      delay(10000);
      ESP.restart();
   }
   digitalWrite(LED_RED_BUILDIN, HIGH);
}

void loop()
{

   // lcd_dev.lcd_set_color(COLOR_WHITE);
   // esp_now_echo();
   delay(10000);
   // broadcast("HELLO!");

   // uint8_t current_target[] = {0x84, 0xF7, 0x03, 0xDC, 0xF6, 0x60};
   uint8_t current_target[] = {0x84, 0xF7, 0x03, 0xDC, 0xF5, 0xC0};
   // // USBSerial.printf("START\n");
   esp_now_test_latency(5000, 125, current_target);

   // USBSerial.printf("\n");
   // int64_t time = esp_timer_get_time();
   // USBSerial.printf("Time since start: %d\n", time);
}