#include "main.h"
#include "board.h"
#include "callback.h"
#include "utils.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// variables
uint16_t color;

void receive_callback(const uint8_t *mac_addr, const uint8_t *data,
                      int data_len) {}

void setup() {

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
    esp_now_register_send_cb(sentCallback);
  } else {
    USBSerial.printf("ERROR: Can't initialize ESP NOW!\n");
    delay(3000);
    ESP.restart();
  }
}
void loop() {

  // lcd_dev.lcd_set_color(COLOR_WHITE);
  delay(2000);
  USBSerial.printf("HELLO!\n");
}