#include "sync-time.h"
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
#include <freertos/queue.h>
#include <freertos/task.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PRIORITY_MESSAGE_INIT_RTT 2
#define PRIORITY_MESSAGE_INIT_TIME 2
#define ESPNOW_QUEUE_SIZE 10
#define ESPNOW_MAXDELAY 512

#define IS_MASTER
#define IS_SLAVE

static const char *TAG = "MAIN";

uint64_t time_corection;
static QueueHandle_t espnow_queue;

static void IRAM_ATTR gpio_handler_isr(void *)
{
   // uint32_t tmp = xthal_get_ccount();
   // start_time = esp_timer_get_time();
}

static void measure_espnow_send_cb(const uint8_t *mac_addr,
                                   esp_now_send_status_t status)
{
   espnow_event_t evt;
   espnow_event_send_cb_t *send_cb = &evt.info.send_cb;

   // Check arg error
   if (mac_addr == NULL) {
      ESP_LOGE(TAG, "Send cb arg error");
      return;
   }

   // Copy data
   evt.id = ESPNOW_SEND_CB;
   memcpy(send_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
   send_cb->status = status;

   // Push to queue
   if (xQueueSend(espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE)
      ESP_LOGW(TAG, "Send se d event into the queue fail");
}

static void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info,
                           const uint8_t *data, int data_len)
{
   espnow_event_t evt;
   espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;
   uint8_t *mac_addr = esp_now_info->src_addr;

   // Check args error
   if (mac_addr == NULL || data == NULL || data_len <= 0) {
      ESP_LOGE(TAG, "Receive cb args error");
      return;
   }

   // Copy data
   evt.id = ESPNOW_RECV_CB;
   memcpy(recv_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
   recv_cb->data = malloc(data_len);
   if (recv_cb->data == NULL) {
      ESP_LOGE(TAG, "Malloc receive data fail");
      return;
   }
   memcpy(recv_cb->data, data, data_len);
   recv_cb->data_len = data_len;
   if (xQueueSend(espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE)
      ESP_LOGW(TAG, "Send receive event into the queue fail");
}

static uint64_t get_time(void) { return esp_timer_get_time() - time_corection; }

void init_rtt_message_task(void)
{
   // Add peers
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
   message_t message;
   message.type = RTT_CAL;
   while (1) {
      message.content = get_time();
      ESP_ERROR_CHECK(
          esp_now_send(mac_addr_2, (uint8_t *)&message, sizeof(message_t)));
      vTaskDelay(50 / portTICK_PERIOD_MS);

      message.content = get_time();
      ESP_ERROR_CHECK(
          esp_now_send(mac_addr_3, (uint8_t *)&message, sizeof(message_t)));
      vTaskDelay(50 / portTICK_PERIOD_MS);
   }
}

void init_time_message_task(void)
{
   // Add peers
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
   message_t message;
   message.type = TIME;
   while (1) {
      message.content = get_time();
      ESP_ERROR_CHECK(
          esp_now_send(mac_addr_2, (uint8_t *)&message, sizeof(message_t)));
      vTaskDelay(250 / portTICK_PERIOD_MS);

      message.content = get_time();
      ESP_ERROR_CHECK(
          esp_now_send(mac_addr_3, (uint8_t *)&message, sizeof(message_t)));
      vTaskDelay(250 / portTICK_PERIOD_MS);
   }
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
   ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
   ESP_ERROR_CHECK(esp_now_register_send_cb(measure_espnow_send_cb));

   BaseType_t init_rtt_message;
   BaseType_t init_time_message;

   espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));

   while (1) {
      init_rtt_message =
          xTaskCreate(init_rtt_message_task, "init_rtt_message_task", 4096,
                      NULL, PRIORITY_MESSAGE_INIT_RTT, NULL);
      init_time_message =
          xTaskCreate(init_time_message_task, "init_time_message_task", 4096,
                      NULL, PRIORITY_MESSAGE_INIT_TIME, NULL);

      vTaskDelay(200);
   }

   // Ending rutine
   printf("Restarting now!\n");
   fflush(stdout);
   esp_restart();
}