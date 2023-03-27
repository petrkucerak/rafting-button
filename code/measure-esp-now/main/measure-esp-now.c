#include "measure-esp-now.h"
#include "esp_chip_info.h"
#include "esp_log.h"
#include "esp_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>

void wifi_init()
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

void measure_broadcast()
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

   /* Print chip information */
   esp_chip_info_t chip_info;
   uint32_t flash_size;
   esp_chip_info(&chip_info);
   printf("This is %s chip with %d CPU core(s), WiFi%s%s, ", CONFIG_IDF_TARGET,
          chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
          (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

   unsigned major_rev = chip_info.revision / 100;
   unsigned minor_rev = chip_info.revision % 100;
   printf("silicon revision v%d.%d, ", major_rev, minor_rev);
   if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
      printf("Get flash size failed");
      return;
   }

   printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
          (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                        : "external");

   printf("Minimum free heap size: %" PRIu32 " bytes\n",
          esp_get_minimum_free_heap_size());

   for (int i = 10; i >= 0; i--) {
      printf("Restarting in %d seconds...\n", i);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
   }
   printf("Restarting now.\n");
   fflush(stdout);
   esp_restart();
}