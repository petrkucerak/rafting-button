#include "main.h"
#include "LCD096.h"
#include "USB.h"
#include "WiFi.h"
#include "board.h"
#include "utils.h"
#include "16bitColors.h"
#include <Arduino.h>

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC USBSerial;
#endif

uint16_t color;

static void usbEventCallback(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
    case ARDUINO_USB_STARTED_EVENT:
      HWSerial.println("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      HWSerial.println("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n",
                      data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      HWSerial.println("USB RESUMED");
      break;

    default:
      break;
    }
  } else if (event_base == ARDUINO_USB_CDC_EVENTS) {
    arduino_usb_cdc_event_data_t *data =
        (arduino_usb_cdc_event_data_t *)event_data;
    switch (event_id) {
    case ARDUINO_USB_CDC_CONNECTED_EVENT:
      HWSerial.println("CDC CONNECTED");
      break;
    case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
      HWSerial.println("CDC DISCONNECTED");
      break;
    case ARDUINO_USB_CDC_LINE_STATE_EVENT:
      HWSerial.printf("CDC LINE STATE: dtr: %u, rts: %u\n",
                      data->line_state.dtr, data->line_state.rts);
      break;
    case ARDUINO_USB_CDC_LINE_CODING_EVENT:
      HWSerial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, "
                      "stop_bits: %u, parity: %u\n",
                      data->line_coding.bit_rate, data->line_coding.data_bits,
                      data->line_coding.stop_bits, data->line_coding.parity);
      break;
    case ARDUINO_USB_CDC_RX_EVENT:
      HWSerial.printf("CDC RX [%u]:", data->rx.len);
      {
        uint8_t buf[data->rx.len];
        size_t len = USBSerial.read(buf, data->rx.len);
        HWSerial.write(buf, len);
      }
      HWSerial.println();
      break;

    default:
      break;
    }
  }
}

void setup() {
  all_pins_init();
  HWSerial.begin(115200);
  HWSerial.setDebugOutput(true);

  USB.onEvent(usbEventCallback);
  USBSerial.onEvent(usbEventCallback);

  USBSerial.begin();
  USB.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  USBSerial.printf("Setup done");

  lcd_dev.lcd_init();
}

void loop() {

  lcd_dev.lcd_set_color(COLOR_WHITE);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_BLACK);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_GREEN);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_BLUE);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_RED);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_YELLOW);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_CYAN);
  delay(1000);
  lcd_dev.lcd_set_color(COLOR_MAGENTA);
  delay(1000);
}