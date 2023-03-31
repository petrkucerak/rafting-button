#include "measure-esp-now.h"
#include "cmp.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>

static void wifi_init(void)
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

static esp_err_t custom_espnow_init(void)
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

static esp_err_t custom_espnow_deinit(void)
{
   ESP_ERROR_CHECK(esp_now_deinit());
   return ESP_OK;
}

void measure_broadcast_task(void)
{
   // 1. init wifi
   // 2. init esp-now
   // 3. register callbacks
   // 4. run broadcast
   // 5. print the report
}

void app_main(void)
{
   // Initialize NVS
   esp_err_t ret = nvs_flash_init();
   if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
       ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
   }
   ESP_ERROR_CHECK(ret);

   printf("The application to measure ESP-NOW latency has been started\n");
   wifi_init();
   printf("Wi-Fi has been started successful\n");
   custom_espnow_init();
   printf("ESP-NOW has been inicialized successfull\n");
   custom_espnow_deinit();
   printf("ESP-NOW has been deinicialized successfull\n");

   cmp_hello();

   // Reset process with delay
   for (int i = 20; i >= 0; i -= 2) {
      printf("Restarting in %d seconds...\n", i);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }
   printf("Restarting now.\n");
   fflush(stdout);
   esp_restart();
}