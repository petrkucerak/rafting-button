#include "esp-now-parameters.h"
#include <driver/gpio.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_intr_alloc.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <nvs_flash.h>
#include <sdkconfig.h>
// #include <soc.h> // defines interrupts
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

uint64_t cyrcle;

static void IRAM_ATTR gpio_handler_isr(void *) { cyrcle = 0; }

void app_main(void)
{
   // Init NVS
   esp_err_t ret = nvs_flash_init();
   if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
       ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
   }
   ESP_ERROR_CHECK(ret);

   gpio_set_direction(21, GPIO_MODE_INPUT);
   ret = esp_intr_alloc(21, 0, gpio_handler_isr, NULL, NULL);
   if (ret != ESP_OK) {
      ESP_ERROR_CHECK(ret);
   }

   cyrcle = 0;
   while (1) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      printf("%lld s\n", cyrcle);
      ++cyrcle;
   }

   // Ending rutine
   printf("Restarting now!\n");
   fflush(stdout);
   esp_restart();
}