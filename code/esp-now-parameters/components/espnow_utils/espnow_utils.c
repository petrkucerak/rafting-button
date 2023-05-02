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
   ESP_ERROR_CHECK(esp_netif_init()); // init  TCP/IP stack layer

   // create loop for system task like Wi-Fi
   ESP_ERROR_CHECK(esp_event_loop_create_default());

   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // init Wi-Fi with default conf

   ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

   // Set current WiFi power save type
   ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_start());
   // ESP_ERROR_CHECK(esp_wifi_set_channel())
}

esp_err_t custom_espnow_init(void)
{

   // Initialzie ESP-NOW
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
   ESP_LOGI(TAG, "Device MAC addres is:" MACSTR "\n", MAC2STR(mac_addr));
}