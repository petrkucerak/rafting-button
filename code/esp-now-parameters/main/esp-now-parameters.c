#include "esp-now-parameters.h"
#include "espnow_utils.h"
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
#include "peripheral.h"
#include <esp_mac.h>
#include <esp_now.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define IS_MASTER

static const char *TAG = "MAIN";

uint64_t start_time;
uint8_t time_reseted;

static void IRAM_ATTR gpio_handler_isr(void *)
{
   // uint32_t tmp = xthal_get_ccount();
   start_time = esp_timer_get_time();
   time_reseted = 1;
}

static void measure_espnow_send_cb(const uint8_t *mac_addr,
                                   esp_now_send_status_t status)
{
   do_blick(10);
   if (status != ESP_OK)
      ESP_LOGW(TAG, "The target " MACSTR " didn't receive a message",
               MAC2STR(mac_addr));
}

static void measure_espnow_recv_cb(const esp_now_recv_info_t *esp_now_info,
                                   const uint8_t *data, int data_len)
{
   uint64_t loc_time = esp_timer_get_time() - start_time;
   do_blick(10);
   uint64_t send_time;
   memcpy(&send_time, data, data_len);
   printf("%lld\n", loc_time - send_time);
}

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

   // SET UP INTERRUPT
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

   // Print device MAC address
   print_mac_address();

   // Set up GPIO_NUM_23 for LED blink
   config_led(GPIO_NUM_23);

   // Init ESP-NOW
   wifi_init();
   ESP_ERROR_CHECK(esp_now_init());
   ESP_ERROR_CHECK(esp_now_register_recv_cb(measure_espnow_recv_cb));
   ESP_ERROR_CHECK(esp_now_register_send_cb(measure_espnow_send_cb));

#ifdef IS_MASTER
   esp_now_peer_info_t peer_info_2 = {};
   memcpy(&peer_info_2.peer_addr, mac_addr_2, 6);
   if (!esp_now_is_peer_exist(mac_addr_2)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_2));
   }
   esp_now_peer_info_t peer_info_3 = {};
   memcpy(&peer_info_3.peer_addr, mac_addr_3, 6);
   if (!esp_now_is_peer_exist(mac_addr_3)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_3));
   }
#endif // IS_MASTER

   uint64_t message;
   while (1) {

#ifdef IS_MASTER
      if (time_reseted) {
         // First batch
         message = esp_timer_get_time() - start_time;
         ESP_ERROR_CHECK(esp_now_send(mac_addr_2, (uint8_t *)&message, 8));
         vTaskDelay(100 / portTICK_PERIOD_MS);
         message = esp_timer_get_time() - start_time;
         ESP_ERROR_CHECK(esp_now_send(mac_addr_3, (uint8_t *)&message, 8));
         vTaskDelay(100 / portTICK_PERIOD_MS);

         // Second batch
         message = esp_timer_get_time() - start_time;
         ESP_ERROR_CHECK(esp_now_send(mac_addr_2, (uint8_t *)&message, 8));
         vTaskDelay(100 / portTICK_PERIOD_MS);
         message = esp_timer_get_time() - start_time;
         ESP_ERROR_CHECK(esp_now_send(mac_addr_3, (uint8_t *)&message, 8));
         vTaskDelay(100 / portTICK_PERIOD_MS);

         // Third batch
         message = esp_timer_get_time() - start_time;
         ESP_ERROR_CHECK(esp_now_send(mac_addr_2, (uint8_t *)&message, 8));
         vTaskDelay(100 / portTICK_PERIOD_MS);
         message = esp_timer_get_time() - start_time;
         ESP_ERROR_CHECK(esp_now_send(mac_addr_3, (uint8_t *)&message, 8));
         vTaskDelay(100 / portTICK_PERIOD_MS);

         // Reset time
         time_reseted = 0;
      }
#endif // IS_MASTER
      #ifndef IS_MASTER
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      #endif // IS_MASTER

   }

   // Ending rutine
   printf("Restarting now!\n");
   fflush(stdout);
   esp_restart();
}