#include "espnow_utils.h"
#include <esp_now.h>
#include <esp_wifi.h>

void wifi_init(void)
{
   ESP_ERROR_CHECK(esp_netif_init()); // init  TCP/IP stack layer

   // create loop for system task like Wi-Fi
   ESP_ERROR_CHECK(esp_event_loop_create_default());

   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // init Wi-Fi with default conf

   ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_start());
   // ESP_ERROR_CHECK(esp_wifi_set_channel())
}

static void espnow_send_cb(const uint8_t *mac_addr,
                           esp_now_send_status_t status)
{
   printf("ESP-NOW send callback has been called\n");
}

static void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info,
                           const uint8_t *data, int data_len)
{
   printf("ESP-NOW receive callback has been called\n");
}

esp_err_t custom_espnow_init(void)
{

   // Initialzie ESP-NOW
   ESP_ERROR_CHECK(esp_now_init());

   // Register ESP-NOW callbacks
   ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
   ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

   // Get the ESP-NOW version, not works
   uint32_t version;
   ESP_ERROR_CHECK(esp_now_get_version(&version));
   printf("ESP-NOW version is: %ld\n", version);

   return ESP_OK;
}

esp_err_t custom_espnow_deinit(void)
{
   ESP_ERROR_CHECK(esp_now_deinit());
   return ESP_OK;
}