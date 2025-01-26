/**
 * @file espnow_utils.c
 * @brief Contains supported functions for the ESP-NOW protocol.
 * @details This file provides functions for initializing and deinitializing
 * ESP-NOW, initializing Wi-Fi, printing the device's MAC address, and comparing
 * MAC addresses.
 * @version 1.0
 * @date 2023-06-08
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 *
 * @note This code is subject to the terms of the MIT license.
 *
 */

#include "espnow_utils.h"
#include "peripheral.h"
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TRUE 1
#define FALSE 0

static const char *TAG = "ESPNOW_UTILS";

void wifi_init(void)
{
   // 1. Initialize TCP/IP stack layer
   ESP_ERROR_CHECK(esp_netif_init());

   // 2. Create a loop for system tasks like Wi-Fi
   ESP_ERROR_CHECK(esp_event_loop_create_default());

   // 3. Initialize Wi-Fi with default configuration
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&cfg));

   // 4. Set storage
   ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

   // 5. Set current WiFi power save type
   ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

   // 6. Set Wi-Fi mode
   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_start());
   // ESP_ERROR_CHECK(esp_wifi_set_channel())
}

esp_err_t custom_espnow_init(void)
{
   // Initialize ESP-NOW
   ESP_ERROR_CHECK(esp_now_init());

   return ESP_OK;
}

esp_err_t custom_espnow_deinit(void)
{
   ESP_ERROR_CHECK(esp_now_deinit());

   return ESP_OK;
}

void print_mac_address()
{
   uint8_t mac_addr[6];
   ESP_ERROR_CHECK(esp_read_mac(mac_addr, ESP_MAC_BASE));
   ESP_LOGI(TAG, "Device MAC address is: " MACSTR "\n", MAC2STR(mac_addr));
}

uint8_t is_device_mac(uint8_t *mac_addr)
{
   uint8_t tmp[6];
   ESP_ERROR_CHECK(esp_read_mac(tmp, ESP_MAC_BASE));
   for (uint8_t i = 0; i < 6; ++i) {
      if (tmp[i] != mac_addr[i])
         return FALSE;
   }
   return TRUE;
}

uint8_t is_same_mac(uint8_t *mac_1, uint8_t *mac_2)
{
   for (uint8_t i = 0; i < ESP_NOW_ETH_ALEN; ++i) {
      if (mac_1[i] != mac_2[i])
         return FALSE;
   }
   return TRUE;
}