#include "main.h"
#include "LCD096.h"
#include "USB.h"
#include "WiFi.h"
#include "board.h"
#include <Arduino.h>

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC USBSerial;
#endif

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
  led_init();
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
}

void loop() {
  //  USBSerial.printf("This is a test\r\n");

  // led_init();
  // led_test();

  lcd_dev.lcd_init();
  lcd_dev.lcd_test();
  delay(1000);
  lcd_dev.lcd_reset();
  delay(1000);
}

void led_init() {
  pinMode(TEST_GPIO2, OUTPUT);
  pinMode(TEST_GPIO3, OUTPUT);
  pinMode(TEST_GPIO4, OUTPUT);
  pinMode(TEST_GPIO5, OUTPUT);
  pinMode(TEST_GPIO6, OUTPUT);
  pinMode(TEST_GPIO7, OUTPUT);
  pinMode(TEST_GPIO8, OUTPUT);

  pinMode(TEST_GPIO10, OUTPUT);
  pinMode(TEST_GPIO11, OUTPUT);
  pinMode(TEST_GPIO12, OUTPUT);
  pinMode(TEST_GPIO13, OUTPUT);
  pinMode(TEST_GPIO14, OUTPUT);
  pinMode(TEST_GPIO15, OUTPUT);
  pinMode(TEST_GPIO16, OUTPUT);
  pinMode(TEST_GPIO17, OUTPUT);

  pinMode(TEST_GPIO34, OUTPUT);
  pinMode(TEST_GPIO35, OUTPUT);
  pinMode(TEST_GPIO36, OUTPUT);
  pinMode(TEST_GPIO37, OUTPUT);
  pinMode(TEST_GPIO38, OUTPUT);
  pinMode(TEST_GPIO39, OUTPUT);
  pinMode(TEST_GPIO40, OUTPUT);
  pinMode(TEST_GPIO41, OUTPUT);
  pinMode(TEST_GPIO42, OUTPUT);
  pinMode(TEST_GPIO43, OUTPUT);
  pinMode(TEST_GPIO44, OUTPUT);
}

void led_test() {
  digitalWrite(TEST_GPIO2, HIGH);
  digitalWrite(TEST_GPIO3, HIGH);
  digitalWrite(TEST_GPIO4, HIGH);
  digitalWrite(TEST_GPIO5, HIGH);
  digitalWrite(TEST_GPIO6, HIGH);
  digitalWrite(TEST_GPIO7, HIGH);
  digitalWrite(TEST_GPIO8, HIGH);

  digitalWrite(TEST_GPIO10, HIGH);
  digitalWrite(TEST_GPIO11, HIGH);
  digitalWrite(TEST_GPIO12, HIGH);
  digitalWrite(TEST_GPIO13, HIGH);
  digitalWrite(TEST_GPIO14, HIGH);
  digitalWrite(TEST_GPIO15, HIGH);
  digitalWrite(TEST_GPIO16, HIGH);
  digitalWrite(TEST_GPIO17, HIGH);

  digitalWrite(TEST_GPIO34, HIGH);
  digitalWrite(TEST_GPIO35, HIGH);
  digitalWrite(TEST_GPIO36, HIGH);
  digitalWrite(TEST_GPIO37, HIGH);
  digitalWrite(TEST_GPIO38, HIGH);
  digitalWrite(TEST_GPIO39, HIGH);
  digitalWrite(TEST_GPIO40, HIGH);
  digitalWrite(TEST_GPIO41, HIGH);
  digitalWrite(TEST_GPIO42, HIGH);
  digitalWrite(TEST_GPIO43, HIGH);
  digitalWrite(TEST_GPIO44, HIGH);

  delay(1000);

  digitalWrite(TEST_GPIO2, LOW);
  digitalWrite(TEST_GPIO3, LOW);
  digitalWrite(TEST_GPIO4, LOW);
  digitalWrite(TEST_GPIO5, LOW);
  digitalWrite(TEST_GPIO6, LOW);
  digitalWrite(TEST_GPIO7, LOW);
  digitalWrite(TEST_GPIO8, LOW);

  digitalWrite(TEST_GPIO10, LOW);
  digitalWrite(TEST_GPIO11, LOW);
  digitalWrite(TEST_GPIO12, LOW);
  digitalWrite(TEST_GPIO13, LOW);
  digitalWrite(TEST_GPIO14, LOW);
  digitalWrite(TEST_GPIO15, LOW);
  digitalWrite(TEST_GPIO16, LOW);
  digitalWrite(TEST_GPIO17, LOW);

  digitalWrite(TEST_GPIO34, LOW);
  digitalWrite(TEST_GPIO35, LOW);
  digitalWrite(TEST_GPIO36, LOW);
  digitalWrite(TEST_GPIO37, LOW);
  digitalWrite(TEST_GPIO38, LOW);
  digitalWrite(TEST_GPIO39, LOW);
  digitalWrite(TEST_GPIO40, LOW);
  digitalWrite(TEST_GPIO41, LOW);
  digitalWrite(TEST_GPIO42, LOW);
  digitalWrite(TEST_GPIO43, LOW);
  digitalWrite(TEST_GPIO44, LOW);

  delay(2000);
}
