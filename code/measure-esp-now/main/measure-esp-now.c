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

void measure_unicast_task_master(void)
{
   // 1. init wifi
   wifi_init();

   // 2. init esp-now
   custom_espnow_init();
   // 3. register callbacks (aboth blick)
   ESP_ERROR_CHECK(esp_now_register_recv_cb(blick_espnow_recv_cb));
   ESP_ERROR_CHECK(esp_now_register_send_cb(blick_espnow_send_cb));

   config_led(GPIO_NUM_23);

   // 4. run broadcast

   // add peer
   uint8_t broadcast_addres[] = {0xc8, 0xf0, 0x9e, 0x7b, 0x10, 0x8c};
   esp_now_peer_info_t peer_info = {};
   memcpy(&peer_info.peer_addr, broadcast_addres, 6);
   esp_err_t result;
   if (!esp_now_is_peer_exist(broadcast_addres)) {
      result = esp_now_add_peer(&peer_info);
      if (result == ESP_ERR_ESPNOW_NOT_INIT) {
         printf("ESP_ERR_ESPNOW_NOT_INIT\n");
      } else if (result == ESP_ERR_ESPNOW_ARG) {
         printf("ESP_ERR_ESPNOW_ARG\n");
      } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
         printf("ESP_ERR_ESPNOW_NOT_FOUND\n");
      } else if (result == ESP_OK) {
         printf("Peers added OK\n");
      } else {
         printf("Another problem\n");
      }
   } else {
      printf("Peers exits");
   }

   // sent message
   uint8_t message[] = {'A', 'h', 'o', 'j', '\0'};
   for (uint8_t i = 0; i < 30; ++i) {

      turn_on_buildin_led();
      turn_on_led(GPIO_NUM_23);

      result = esp_now_send(broadcast_addres, &message, 5);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }

   // 5. print the report?

   custom_espnow_deinit();
}

void measure_broadcast_task_master(void)
{
   // 1. init wifi
   wifi_init();

   // 2. init esp-now
   custom_espnow_init();
   // 3. register callbacks (aboth blick)
   ESP_ERROR_CHECK(esp_now_register_recv_cb(blick_espnow_recv_cb));
   ESP_ERROR_CHECK(esp_now_register_send_cb(blick_espnow_send_cb));

   // 4. run broadcast

   // add peer
   uint8_t broadcast_addres[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
   esp_now_peer_info_t peer_info = {};
   memcpy(&peer_info.peer_addr, broadcast_addres, 6);
   esp_err_t result;
   if (!esp_now_is_peer_exist(broadcast_addres)) {
      result = esp_now_add_peer(&peer_info);
      if (result == ESP_ERR_ESPNOW_NOT_INIT) {
         printf("ESP_ERR_ESPNOW_NOT_INIT\n");
      } else if (result == ESP_ERR_ESPNOW_ARG) {
         printf("ESP_ERR_ESPNOW_ARG\n");
      } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
         printf("ESP_ERR_ESPNOW_NOT_FOUND\n");
      } else if (result == ESP_OK) {
         printf("Peers added OK\n");
      } else {
         printf("Another problem\n");
      }
   } else {
      printf("Peers exits");
   }

   // sent message
   uint8_t message[] = {'A', 'h', 'o', 'j', '\0'};
   for (uint8_t i = 0; i < 30; ++i) {

      BaseType_t blick_task;
      blick_task = xTaskCreate((void *)do_blick_task, "t_do_blick_send", 4096,
                               (void *)10, tskIDLE_PRIORITY, NULL);

      result = esp_now_send(broadcast_addres, &message, 5);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }

   // 5. print the report?

   custom_espnow_deinit();
}
void measure_broadcast_task_slave(void)
{
   // 1. init wifi
   wifi_init();

   // 2. init esp-now
   custom_espnow_init();
   // 3. register callbacks (aboth blick)
   ESP_ERROR_CHECK(esp_now_register_recv_cb(blick_espnow_recv_cb));
   ESP_ERROR_CHECK(esp_now_register_send_cb(blick_espnow_send_cb));

   while (1) {
      vTaskDelay(1000);
   }

   // 5. print the report?

   custom_espnow_deinit();
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

   // wifi_init();
   // uint8_t mac[6];
   // esp_wifi_get_mac(WIFI_IF_STA, &mac);

   // printf("The mac adress is: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", mac[0],
   // mac[1],
   //        mac[2], mac[3], mac[4], mac[5]);

   measure_broadcast_task_master();
   // measure_broadcast_task_slave();
   // measure_unicast_task_master();
   // config_led(GPIO_NUM_23);
   // Reset process with delay
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