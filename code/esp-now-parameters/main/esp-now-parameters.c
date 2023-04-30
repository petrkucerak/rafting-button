#include "esp-now-parameters.h"
#include <driver/gpio.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_intr_alloc.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <nvs_flash.h>
#include <sdkconfig.h>
// #include <soc.h> // defines interrupts
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint64_t start_time;
esp_timer_handle_t timer_handle;

static void IRAM_ATTR gpio_handler_isr(void *)
{
   esp_timer_restart(timer_handle, 1000);
   start_time = esp_timer_get_time();
}

static void timer_callback(void *arg) { ++time; }

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

   // Reset pint
   ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_21));
   // Set intr type
   ESP_ERROR_CHECK(gpio_set_intr_type(GPIO_NUM_21, GPIO_INTR_POSEDGE));
   // Eneable intr
   ESP_ERROR_CHECK(gpio_intr_enable(GPIO_NUM_21));
   // Set gpio direction
   ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_21, GPIO_MODE_INPUT));
   // Install isr service
   ESP_ERROR_CHECK(gpio_install_isr_service(0));
   // Add isr handler
   ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_NUM_21, gpio_handler_isr, NULL));

   // set up timer
   esp_timer_create_args_t timer_args = {};
   timer_args.name = "timer";
   timer_args.callback = &timer_callback;

   ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));

   // Periodic esp_timer also imposes a 50us restriction on the minimal timer
   // period. Periodic software timers with period of less than 50us are not
   // practical since they would consume most of the CPU time.
   ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000));

   while (1) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      printf("%lld\n", time);
   }

   // Ending rutine
   printf("Restarting now!\n");
   fflush(stdout);
   esp_restart();
}