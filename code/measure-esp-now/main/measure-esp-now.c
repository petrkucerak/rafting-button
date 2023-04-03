#include "measure-esp-now.h"
#include "espnow_utils.h"
#include "peripheral.h"
#include <driver/gpio.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <nvs_flash.h>
#include <sdkconfig.h>
#include <stdio.h>

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

   config_led(GPIO_NUM_23);

   // Reset process with delay
   for (int i = 20; i >= 0; i -= 2) {
      printf("Restarting in %d seconds...\n", i);
      turn_on_led(GPIO_NUM_23);
      turn_on_buildin_led();
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      turn_off_led(GPIO_NUM_23);
      turn_off_buildin_led();
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }
   printf("Restarting now.\n");
   fflush(stdout);
   esp_restart();
}