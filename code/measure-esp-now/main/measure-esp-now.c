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
#include <string.h>

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

   wifi_init();
   custom_espnow_init();
   custom_espnow_deinit();


   for (int i = 10; i >= 0; i -= 2) {
      printf("Restarting in %d cycles...\n", i);
      turn_on_led(GPIO_NUM_23);
      turn_on_buildin_led();
      vTaskDelay(200 / portTICK_PERIOD_MS);
      turn_off_led(GPIO_NUM_23);
      turn_off_buildin_led();
      vTaskDelay(200 / portTICK_PERIOD_MS);
   }
   printf("Restarting now.\n");
   fflush(stdout);
   esp_restart();
}