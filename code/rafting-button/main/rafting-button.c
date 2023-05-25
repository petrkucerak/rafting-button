#include "rafting-button.h"
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
#define STACK_SIZE 4096

#define DEVIATION_LIMIT 200
#define DEVIATION_MAX_CONSTANT 25

#define CONFIG_ESPNOW_SEND_LEN 250

// #define IS_MASTER
#define IS_SLAVE

static const char *TAG = "MAIN";
static uint8_t s_broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF,
                                                    0xFF, 0xFF, 0xFF};
esp_now_peer_info_t broadcast_peer;

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

static uint64_t get_time_with_timer(uint64_t esp_time)
{
   return esp_time - node.time_corection;
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
   // mark event by timestamp
   evt.timestamp = esp_timer_get_time();
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
   // mark event by timestamp
   evt.timestamp = esp_timer_get_time();
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
                      uint64_t *content, uint32_t *epoch_id,
                      neighbour_t *neighbour)
{
   message_data_t *buf = (message_data_t *)data;

   if (data_len < sizeof(message_data_t)) {
      ESP_LOGE(TAG, "Receive ESPNOW data is too short, len:%d", data_len);
      return -1;
   }

   *type = buf->type;
   *content = buf->content;
   *epoch_id = buf->epoch_id;
   *neighbour = buf->neighbour[0];
   return buf->type;
}

void espnow_data_prepare(espnow_send_param_t *send_param)
{
   message_data_t *buf = (message_data_t *)send_param->buf;

   send_param->data_len = sizeof(message_data_t);
   buf->type = send_param->type;
   buf->epoch_id = send_param->epoch_id;
   buf->content = send_param->content;
   memcpy(&buf->neighbour[0], &send_param->neighbour[0],
          sizeof(neighbour_t) * NEIGHBOURS_COUNT);
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

void handle_espnow_add_peer_error(esp_err_t code)
{
   if (code == ESP_ERR_ESPNOW_NOT_INIT)
      ESP_LOGE(TAG, "ESPNOW is not initialized");
   else if (code == ESP_ERR_ESPNOW_ARG)
      ESP_LOGE(TAG, "invalid argument");
   else if (code == ESP_ERR_ESPNOW_FULL)
      ESP_LOGE(TAG, "peer list is full");
   else if (code == ESP_ERR_ESPNOW_NO_MEM)
      ESP_LOGE(TAG, "out of memory");
   else if (code == ESP_ERR_ESPNOW_EXIST)
      ESP_LOGE(TAG, "peer has existed");
   else if (code == ESP_OK)
      ESP_LOGI(TAG, "succeed adding peer");
}

void espnow_handler_task(void)
{
   espnow_event_t evt;
   uint64_t content = 0;
   message_type_t type;
   uint32_t epoch_id;
   neighbour_t neighbours[NEIGHBOURS_COUNT];
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
                                 &content, &epoch_id, &neighbours);
         free(recv_cb->data);
         if (epoch_id < node.epoch_id)
            ESP_LOGE(TAG, "Wrong number of epoch ID");
         switch (ret) {
         case HELLO_DS: {
            ESP_LOGI(TAG, "Receive HELLO_DS");
            // add device to my list or make it ACTIVE
            if (!esp_now_is_peer_exist(recv_cb->mac_addr)) {
               uint8_t i = 0;
               while (node.neighbour[i].status != INACTIVE)
                  ++i;
               esp_now_peer_info_t peer_info = {};
               memcpy(&peer_info.peer_addr, recv_cb->mac_addr,
                      ESP_NOW_ETH_ALEN);
               ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
               node.neighbour[i].status = ACTIVE;
               node.neighbour[i].title = SLAVE;
               memcpy(&node.neighbour[i].mac_addr, recv_cb->mac_addr,
                      ESP_NOW_ETH_ALEN);
            } else {
               for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
                  if (memcmp(&node.neighbour[i].mac_addr, recv_cb->mac_addr,
                             ESP_NOW_ETH_ALEN) == 0) {
                     node.neighbour[i].status = ACTIVE;
                     break;
                  }
               }
            }
            // send back: neighbour list, epoch id
            ESP_LOGI(TAG, "Send NEIGHBOUR message");
            send_param->type = NEIGHBOURS;
            send_param->epoch_id = node.epoch_id;
            memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
            memcpy(&send_param->neighbour[0], &node.neighbour[0],
                   sizeof(neighbour_t) * NEIGHBOURS_COUNT);
            espnow_data_prepare(send_param);

            ret = esp_now_send(send_param->dest_mac, send_param->buf,
                               send_param->data_len);
            if (ret != ESP_OK)
               handle_espnow_send_error(ret);

         } break;
         case NEIGHBOURS: {
            ESP_LOGI(TAG, "Receive NEIGHBOUR message");
            // save neighbours
            for (uint8_t j = 0; j < NEIGHBOURS_COUNT; ++j) {
               if (!is_device_mac(&neighbours[j].mac_addr)) {
                  if (!esp_now_is_peer_exist(&neighbours[j].mac_addr)) {
                     uint8_t i = 0;
                     while (node.neighbour[i].status != INACTIVE)
                        ++i;
                     esp_now_peer_info_t peer_info = {};
                     memcpy(&peer_info.peer_addr, &neighbours[j].mac_addr,
                            ESP_NOW_ETH_ALEN);
                     ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
                     node.neighbour[i].status = neighbours[j].status;
                     node.neighbour[i].title = neighbours[j].title;
                     memcpy(&node.neighbour[i].mac_addr,
                            &neighbours[j].mac_addr, ESP_NOW_ETH_ALEN);
                  } else {
                     for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
                        if (memcmp(&node.neighbour[i].mac_addr,
                                   &neighbours[j].mac_addr,
                                   ESP_NOW_ETH_ALEN) == 0) {
                           node.neighbour[i].status = neighbours[j].status;
                           node.neighbour[i].title = neighbours[j].title;
                           break;
                        }
                     }
                  }
               }
            }
         } break;
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

void send_hello_ds_message(void)
{
   ESP_LOGI(TAG, "Send HELLO_DS");
   espnow_send_param_t *send_param = NULL;
   send_param = malloc(sizeof(espnow_send_param_t));
   if (send_param == NULL) {
      ESP_LOGE(TAG, "Malloc send parametr fail [Hello DS]");
      return;
   }
   memset(send_param, 0, sizeof(espnow_send_param_t));

   // type | epoch ID | target
   send_param->epoch_id = node.epoch_id;
   memcpy(send_param->dest_mac, s_broadcast_mac, ESP_NOW_ETH_ALEN);
   send_param->type = HELLO_DS;

   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail [Hello DS]");
      free(send_param);
      return;
   }
   esp_err_t ret;
   espnow_data_prepare(send_param);
   ret = esp_now_send(send_param->dest_mac, send_param->buf,
                      send_param->data_len);
   if (ret != ESP_OK)
      handle_espnow_send_error(ret);
   free(send_param);
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

   // Init Wifi
   wifi_init();
   // Init ESP-NOW
   ESP_ERROR_CHECK(esp_now_init());
   ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
   ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));

   // Prepare node strucutre
   node.is_firts_setup_rtt = 1;
   node.rtt_balancer_index = 0;
   node.epoch_id = 0;
   node.is_time_synced = 0;
   for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
      node.neighbour[i].status = NOT_INITIALIZED;
   }

   // Create a broadcast peer
   memcpy(&broadcast_peer.peer_addr, s_broadcast_mac, 6);
   if (!esp_now_is_peer_exist(s_broadcast_mac)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&broadcast_peer));
   }

   espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));

   BaseType_t handler_task;
   handler_task =
       xTaskCreate((TaskFunction_t)espnow_handler_task, "espnow_handler_task",
                   STACK_SIZE, NULL, PRIORITY_HANDLER, NULL);

   send_hello_ds_message();

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
