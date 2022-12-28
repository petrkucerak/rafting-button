#include "main.h"
#include "board.h"
#include "callback.h"
#include "utils.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// variables
uint16_t color;

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen) {
  // Only allow a maximum of 250 characters in the message + a null terminating
  // byte
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msg_len = min(ESP_NOW_MAX_DATA_LEN, dataLen);
  strncpy(buffer, (const char *)data, msg_len);

  // Make sure we are null terminated
  buffer[msg_len] = 0;
}

void setup() {

  all_pins_init();

  HWSerial.begin(115200);
  HWSerial.setDebugOutput(true);

  USB.onEvent(usbEventCallback);
  USBSerial.onEvent(usbEventCallback);

  USBSerial.begin();
  USB.begin();

  // init_usb();
  mac_on_display();

  // config ESP NOW broadcast
  // WiFi.mode(WIFI_STA);
  // WiFi.disconnect();
  // if (esp_now_init() == ESP_OK) {
  //   USBSerial.printf("Initialization ESP NOW has been success!\n");
  //   esp_now_register_recv_cb(receiveCallback);
  //   esp_now_register_send_cb(sentCallback);
  // } else {
  //   USBSerial.printf("ERROR: Can't initialize ESP NOW!\n");
  //   delay(3000);
  //   ESP.restart();
  // }
}
void loop() {

  // lcd_dev.lcd_set_color(COLOR_WHITE);
  delay(2000);
  USBSerial.printf("HELLO!\n");
}