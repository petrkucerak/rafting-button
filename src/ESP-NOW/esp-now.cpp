#include "esp-now.h"

#include "USB-C/USB-C.h"
#include "display/16bit-colors.h"
#include "display/LCD096.h"
#include "utils.h"

/**
 * @brief The function is called when device receive a new ESP NOW message.
 * The function stores information into a special structure called
 * `ESP_NOW HANDLER` and sets variable with status in this function into zero.
 *
 * @param mac_addr
 * @param data
 * @param data_len
 */
void receive_callback(const uint8_t *mac_addr, const uint8_t *data,
                      int data_len);

/**
 * @brief TODO: description
 *
 * @param mac_addr
 * @param status
 */
void sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status);

void format_mac_address(const uint8_t *mac_address, char *buffer, int max_size)
{
   snprintf(buffer, max_size, "%02X:%02X:%02X:%02X:%02X:%02X", mac_address[0],
            mac_address[1], mac_address[2], mac_address[3], mac_address[4],
            mac_address[5]);
}

void sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
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

void receive_callback(const uint8_t *mac_addr, const uint8_t *data,
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

esp_now_s esp_now_dev = {sent_callback, receive_callback};
ESP_NOW_HANDLER esp_now_handler;