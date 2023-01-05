#ifndef __CALLBACK_H
#define __CALLBACK_H

// ~~~ The start of USB Callback ~~~
#include "ESP-NOW/esp-now.h"
#include "utils.h"
#include <Arduino.h>
#include <USB.h>
#include <esp_now.h>

#define TRUE 1
#define FALSE 0

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
static USBCDC USBSerial;
#endif

static void usb_event_callback(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
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
                         data->line_coding.bit_rate,
                         data->line_coding.data_bits,
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
// ~~~ The end of USB Callback ~~~

typedef struct ESP_NOW_HANDLER_S {
   uint8_t sender_mac_addr[6];
   uint8_t isEmpty;
   char data[250];
   uint8_t data_len;
} ESP_NOW_HANDLER;

static ESP_NOW_HANDLER esp_now_handler;

static void sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
   // char mac_addr_string[18];
   // format_mac_address(mac_addr, mac_addr_string, 18);
   // USBSerial.printf("Sent callback to: %s", mac_addr_string);
   // if (status == ESP_NOW_SEND_SUCCESS)
   //    USBSerial.printf(" success\n");
   // else
   //    USBSerial.printf(" failure\n");

   // red_blick(100);
}

/**
 * @brief The function is called when device receive a new ESP NOW message.
 * The function stores information into a special structure called
 * `ESP_NOW HANDLER` and sets variable with status in this function into zero.
 *
 * @param mac_addr
 * @param data
 * @param data_len
 */
static void receive_callback(const uint8_t *mac_addr, const uint8_t *data,
                             int data_len)
{

   // copy mac address
   for (uint8_t i = 0; i < 6; ++i) {
      esp_now_handler.sender_mac_addr[i] = mac_addr[i];
   }

   // char mac_addr_string[18];
   // format_mac_address(mac_addr, mac_addr_string, 18);
   // USBSerial.printf("Recieve callback: %s ", mac_addr_string);

   // copy data
   for (uint8_t i = 0; i < (uint8_t)data_len; ++i) {
      esp_now_handler.data[i] = data[i];
      // USBSerial.printf("%c", data[i]);
   }
   // USBSerial.printf("\n");
   esp_now_handler.data_len = (uint8_t)data_len;
   esp_now_handler.isEmpty = FALSE;

   red_blick(1);
}
// ~~~ The start of ESP NOW Callbacks ~~~

#endif // __CALLBACK_H