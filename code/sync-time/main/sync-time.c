#include "sync-time.h"
#include "espnow_utils.h"
#include "peripheral.h"
#include <assert.h>
#include <driver/gpio.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_intr_alloc.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_now.h>
#include <esp_random.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <nvs_flash.h>
#include <sdkconfig.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// #include <soc.h> // defines interrupts

#define PRIORITY_RTT_START 2
#define PRIORITY_TIME_START 2
#define PRIORITY_HANDLER 3

#define ESPNOW_QUEUE_SIZE 10
#define PRINT_QUEUE_SIZE 4

#define ESPNOW_MAXDELAY 10
#define STACK_SIZE 2048

#define DEVIATION_MAX_CONSTANT 100

#define TIME_ERROR_CONSTANT 300

#define CONFIG_ESPNOW_SEND_LEN 250

// #define IS_MASTER
#define IS_SLAVE

static const char *TAG = "MAIN";

node_info_t node;
static QueueHandle_t espnow_queue;
static QueueHandle_t print_data;

uint32_t get_rtt_avg()
{
   uint32_t avg = 0;
   for (uint16_t i = 0; i < BALANCER_SIZE; ++i) {
      avg += node.rtt_balancer[i];
   }
   return (uint32_t)(avg / BALANCER_SIZE);
}

static uint64_t get_time(void)
{
   return esp_timer_get_time() - node.time_corection;
}

static void IRAM_ATTR gpio_handler_isr(void *)
{
   print_data_t data;
   data.rtt = get_rtt_avg();
   data.deviation = node.deviation_avg;
   data.time = get_time();
   xQueueSendFromISR(print_data, &data, NULL);
}

static void espnow_send_cb(const uint8_t *mac_addr,
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
      ESP_LOGW(TAG, "Send send event into the queue fail");
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

int espnow_data_parse(uint8_t *data, int data_len, message_type_t *type,
                      uint64_t *content)
{
   message_data_t *buf = (message_data_t *)data;

   if (data_len < sizeof(message_data_t)) {
      ESP_LOGE(TAG, "Receive ESPNOW data is too short, len:%d", data_len);
      return -1;
   }

   *type = buf->type;
   *content = buf->content;
   return buf->type;
}

void espnow_data_prepare(espnow_send_param_t *send_param)
{
   message_data_t *buf = (message_data_t *)send_param->buf;

   send_param->data_len = sizeof(message_data_t);
   buf->type = send_param->type;
   buf->content = send_param->content;
   /* Fill all remaining bytes after the data with random values */
   esp_fill_random(buf->payload, send_param->data_len - sizeof(message_data_t));
}

void handle_espnow_send_error(esp_err_t code)
{
   if (code == ESP_OK)
      ESP_LOGI(TAG, "Send is OK");
   else if (code == ESP_ERR_ESPNOW_NOT_INIT)
      ESP_LOGE(TAG, "ESPNOW is not initialized");
   else if (code == ESP_ERR_ESPNOW_ARG)
      ESP_LOGE(TAG, "Invalid argument");
   else if (code == ESP_ERR_ESPNOW_INTERNAL)
      ESP_LOGE(TAG, "Internal error");
   else if (code == ESP_ERR_ESPNOW_NO_MEM)
      ESP_LOGE(TAG, "Out of memory, when this happens, you can delay a while "
                    "before sending the next data");
   else if (code == ESP_ERR_ESPNOW_NOT_FOUND)
      ESP_LOGE(TAG, "Peer is not found");
   else if (code == ESP_ERR_ESPNOW_IF)
      ESP_LOGE(TAG, "Current WiFi interface doesn't match that of peer");
}

void espnow_handler_task(void)
{
   espnow_event_t evt;
   uint64_t content = 0;
   message_type_t type;
   int ret;

   espnow_send_param_t *send_param = NULL;
   send_param = malloc(sizeof(espnow_send_param_t));
   if (send_param == NULL) {
      ESP_LOGE(TAG, "Malloc send parametr fail");
      vSemaphoreDelete(espnow_queue);
      return;
   }
   memset(send_param, 0, sizeof(espnow_send_param_t));
   send_param->content = 0;
   // send_param->dest_mac;
   // send_param->type;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vSemaphoreDelete(espnow_queue);
      return;
   }

   while (xQueueReceive(espnow_queue, &evt, portMAX_DELAY) == pdTRUE) {
      switch (evt.id) {
      case ESPNOW_SEND_CB: {
         // handle send
         espnow_event_send_cb_t *send_cb = &evt.info.send_cb;
         if (send_cb->status != ESP_OK)
            ESP_LOGW(TAG, "Send to " MACSTR " failed",
                     MAC2STR(send_cb->mac_addr));
         break;
      }
      case ESPNOW_RECV_CB: {
         // handle recv
         espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;
         ret = espnow_data_parse(recv_cb->data, recv_cb->data_len, &type,
                                 &content);
         free(recv_cb->data);
         switch (ret) {
         case RTT_CAL_MASTER:
            // send value back to master with type RTT_CAL_SLAVE
            send_param->content = content;
            send_param->type = RTT_CAL_SLAVE;
            memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
            espnow_data_prepare(send_param);
            if (esp_now_send(send_param->dest_mac, send_param->buf,
                             send_param->data_len) != ESP_OK) {
               ESP_LOGW(TAG, "Send RTT_CAL_MASTER error");
            }
            break;
         case RTT_CAL_SLAVE:
            // calcule RTT and send it back to slave with type RTT_VAL
            send_param->content = (get_time() - content) / 2;
            send_param->type = RTT_VAL;
            memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
            espnow_data_prepare(send_param);
            if (esp_now_send(send_param->dest_mac, send_param->buf,
                             send_param->data_len) != ESP_OK) {
               ESP_LOGW(TAG, "Send RTT_CAL_SLAVE error");
            }
            break;
         case RTT_VAL:
            // set RTT value into the array
            if (node.is_firts_setup_rtt) {
               for (uint16_t i = 0; i < BALANCER_SIZE; ++i) {
                  node.rtt_balancer[i] = content;
               }
               node.is_firts_setup_rtt = 0;
            } else {
               node.rtt_balancer[node.rtt_balancer_index] = content;
               ++node.rtt_balancer_index;
               if (node.rtt_balancer_index == BALANCER_SIZE)
                  node.rtt_balancer_index = 0;
            }
            break;
         case TIME:
            // calcule deviation O~
            if (node.is_first_setup_deviation) {
               node.deviation_avg = (int32_t)get_time() - (int32_t)content -
                                    (int32_t)get_rtt_avg();
               node.is_first_setup_deviation = 0;
            } else {
               node.deviation_avg =
                   (node.deviation_avg + (int32_t)get_time() -
                    (int32_t)content - (int32_t)get_rtt_avg()) /
                   2;
            }

            // set time
            if (node.deviation_avg < TIME_ERROR_CONSTANT ||
                node.is_time_synced == 0) {
               if (node.deviation_avg > DEVIATION_MAX_CONSTANT)
                  node.time_corection = esp_timer_get_time() -
                                        (content + (uint64_t)get_rtt_avg() +
                                         DEVIATION_MAX_CONSTANT);
               else if (node.deviation_avg < -DEVIATION_MAX_CONSTANT)
                  node.time_corection = esp_timer_get_time() -
                                        (content + (uint64_t)get_rtt_avg() -
                                         DEVIATION_MAX_CONSTANT);
               else
                  node.time_corection = esp_timer_get_time() -
                                        (content + (uint64_t)get_rtt_avg() +
                                         (uint64_t)node.deviation_avg);

               if (node.deviation_avg < TIME_ERROR_CONSTANT)
                  node.is_time_synced = 1;
            }
            break;
         default:
            ESP_LOGE(TAG, "Receive unknown message type");
            break;
         }
         break;
      }
      default:
         ESP_LOGE(TAG, "Callback type error: %d", evt.id);
         break;
      }
   }
   free(send_param->buf);
   free(send_param);
   vSemaphoreDelete(espnow_queue);
}

void task_start_sync_rtt(void)
{
   espnow_send_param_t *send_param = NULL;
   send_param = malloc(sizeof(espnow_send_param_t));
   if (send_param == NULL) {
      ESP_LOGE(TAG, "Malloc send parametr fail");
      vTaskDelete(NULL);
   }
   memset(send_param, 0, sizeof(espnow_send_param_t));
   send_param->content = 0;
   // send_param->dest_mac;
   // send_param->type;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vTaskDelete(NULL);
   }
   esp_err_t ret;
   vTaskDelay(500 / portTICK_PERIOD_MS);
   while (1) {
      vTaskDelay(25 / portTICK_PERIOD_MS);

      // // Node 2
      // send_param->type = RTT_CAL_MASTER;
      // memcpy(send_param->dest_mac, mac_addr_2, ESP_NOW_ETH_ALEN);
      // send_param->content = get_time();
      // espnow_data_prepare(send_param);

      // ret = esp_now_send(send_param->dest_mac, send_param->buf,
      //                    send_param->data_len);
      // if (ret != ESP_OK)
      //    handle_espnow_send_error(ret);

      // vTaskDelay(25 / portTICK_PERIOD_MS);

      // Node 3
      send_param->type = RTT_CAL_MASTER;
      memcpy(send_param->dest_mac, mac_addr_3, ESP_NOW_ETH_ALEN);
      send_param->content = get_time();
      espnow_data_prepare(send_param);

      ret = esp_now_send(send_param->dest_mac, send_param->buf,
                         send_param->data_len);
      if (ret != ESP_OK)
         handle_espnow_send_error(ret);
      vTaskDelay(25 / portTICK_PERIOD_MS);

      // Node 4
      send_param->type = RTT_CAL_MASTER;
      memcpy(send_param->dest_mac, mac_addr_4, ESP_NOW_ETH_ALEN);
      send_param->content = get_time();
      espnow_data_prepare(send_param);

      ret = esp_now_send(send_param->dest_mac, send_param->buf,
                         send_param->data_len);
      if (ret != ESP_OK)
         handle_espnow_send_error(ret);
      vTaskDelay(25 / portTICK_PERIOD_MS);

      // // Node 5
      // send_param->type = RTT_CAL_MASTER;
      // memcpy(send_param->dest_mac, mac_addr_5, ESP_NOW_ETH_ALEN);
      // send_param->content = get_time();
      // espnow_data_prepare(send_param);

      // ret = esp_now_send(send_param->dest_mac, send_param->buf,
      //                    send_param->data_len);
      // if (ret != ESP_OK)
      //    handle_espnow_send_error(ret);
   }
   free(send_param);
   vTaskDelete(NULL);
}

void task_start_sync_time(void)
{
   espnow_send_param_t *send_param = NULL;
   send_param = malloc(sizeof(espnow_send_param_t));
   if (send_param == NULL) {
      ESP_LOGE(TAG, "Malloc send parametr fail");
      vTaskDelete(NULL);
   }
   memset(send_param, 0, sizeof(espnow_send_param_t));
   send_param->content = 0;
   // send_param->dest_mac;
   // send_param->type;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vTaskDelete(NULL);
   }
   esp_err_t ret;
   vTaskDelay(500 / portTICK_PERIOD_MS);
   while (1) {
      // Node 2
      // vTaskDelay(125 / portTICK_PERIOD_MS);
      // send_param->type = TIME;
      // memcpy(send_param->dest_mac, mac_addr_2, ESP_NOW_ETH_ALEN);
      // send_param->content = get_time();
      // espnow_data_prepare(send_param);
      // ret = esp_now_send(send_param->dest_mac, send_param->buf,
      //                    send_param->data_len);
      // if (ret != ESP_OK)
      //    handle_espnow_send_error(ret);

      // Node 3
      vTaskDelay(125 / portTICK_PERIOD_MS);
      send_param->type = TIME;
      memcpy(send_param->dest_mac, mac_addr_3, ESP_NOW_ETH_ALEN);
      send_param->content = get_time();
      espnow_data_prepare(send_param);
      ret = esp_now_send(send_param->dest_mac, send_param->buf,
                         send_param->data_len);
      if (ret != ESP_OK)
         handle_espnow_send_error(ret);

      // Node 4
      vTaskDelay(125 / portTICK_PERIOD_MS);
      send_param->type = TIME;
      memcpy(send_param->dest_mac, mac_addr_4, ESP_NOW_ETH_ALEN);
      send_param->content = get_time();
      espnow_data_prepare(send_param);
      ret = esp_now_send(send_param->dest_mac, send_param->buf,
                         send_param->data_len);
      if (ret != ESP_OK)
         handle_espnow_send_error(ret);

      // // Node 5
      // vTaskDelay(125 / portTICK_PERIOD_MS);
      // send_param->type = TIME;
      // memcpy(send_param->dest_mac, mac_addr_5, ESP_NOW_ETH_ALEN);
      // send_param->content = get_time();
      // espnow_data_prepare(send_param);
      // ret = esp_now_send(send_param->dest_mac, send_param->buf,
      //                    send_param->data_len);
      // if (ret != ESP_OK)
      //    handle_espnow_send_error(ret);
   }
   free(send_param);
   vTaskDelete(NULL);
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
   ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));

   node.is_first_setup_deviation = 1;
   node.is_firts_setup_rtt = 1;
   node.rtt_balancer_index = 0;
   node.is_time_synced = 0;

   espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));

   // Add peers
   esp_now_peer_info_t peer_info_1 = {};
   memcpy(&peer_info_1.peer_addr, mac_addr_1, 6);
   if (!esp_now_is_peer_exist(mac_addr_1)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_1));
   }
   // esp_now_peer_info_t peer_info_2 = {};
   // memcpy(&peer_info_2.peer_addr, mac_addr_2, 6);
   // if (!esp_now_is_peer_exist(mac_addr_2)) {
   //    ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_2));
   // }
   esp_now_peer_info_t peer_info_3 = {};
   memcpy(&peer_info_3.peer_addr, mac_addr_3, 6);
   if (!esp_now_is_peer_exist(mac_addr_3)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_3));
   }
   esp_now_peer_info_t peer_info_4 = {};
   memcpy(&peer_info_4.peer_addr, mac_addr_4, 6);
   if (!esp_now_is_peer_exist(mac_addr_4)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_4));
   }
   // esp_now_peer_info_t peer_info_5 = {};
   // memcpy(&peer_info_5.peer_addr, mac_addr_5, 6);
   // if (!esp_now_is_peer_exist(mac_addr_5)) {
   //    ESP_ERROR_CHECK(esp_now_add_peer(&peer_info_5));
   // }

   BaseType_t handler_task;
   handler_task =
       xTaskCreate((TaskFunction_t)espnow_handler_task, "espnow_handler_task",
                   STACK_SIZE, NULL, PRIORITY_HANDLER, NULL);

#ifdef IS_MASTER
   // Init MASTER tasks
   BaseType_t start_rtt_sync_task;
   start_rtt_sync_task =
       xTaskCreate((TaskFunction_t)task_start_sync_rtt, "start_sync_rtt",
                   STACK_SIZE, NULL, PRIORITY_RTT_START, NULL);

   BaseType_t start_time_sync_task;
   start_time_sync_task =
       xTaskCreate((TaskFunction_t)task_start_sync_time, "start_sync_time",
                   STACK_SIZE, NULL, PRIORITY_TIME_START, NULL);
#endif // IS_MASTER

   print_data = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(print_data_t));

   print_data_t data;

   vTaskDelay(1000 / portTICK_PERIOD_MS);

   while (xQueueReceive(print_data, &data, portMAX_DELAY) == pdTRUE) {
      printf("%ld,%ld,%lld\n", data.rtt, data.deviation, data.time);
   }

   // Ending rutine
   printf("Restarting now!\n");
   fflush(stdout);
   esp_restart();
}
