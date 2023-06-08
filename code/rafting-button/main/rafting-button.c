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
#define PRIORITY_HANDLER 4
#define PRIORITY_HANDLE_DS_EVENT 2
#define PRIORITY_REQUEST_TASK 2
#define PRIORITY_HANDLE_ISR_EVENT 3

#define ESPNOW_QUEUE_SIZE 10
#define PRINT_QUEUE_SIZE 4
#define ISR_QUEUE_SIZE 1

#define ESPNOW_MAXDELAY 10
#define CLEANING_DELAY 1
#define DS_MAXDELAY 100
#define STACK_SIZE 4096

#define DEVIATION_LIMIT 200
#define DEVIATION_MAX_CONSTANT 25

#define CONFIG_ESPNOW_SEND_LEN 250

#define COUNT_ERROR_MESSAGE_TO_INACTIVE 5

#define VOTE_TIMEOUT 1000000
#define MASTER_TIMEOUT 2000000

// #define IS_MASTER
#define IS_SLAVE

static const char *TAG = "MAIN";
static uint8_t s_broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF,
                                                    0xFF, 0xFF, 0xFF};
esp_now_peer_info_t broadcast_peer;

node_info_t node;
static QueueHandle_t espnow_queue;
static QueueHandle_t log_event;
static QueueHandle_t isr_event;

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

void print_neighbours(void)
{
   printf("\n");
   for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
      ESP_LOGI("NEIGBOURS", MACSTR " | status: %d | title: %d",
               MAC2STR(node.neighbour[i].mac_addr), node.neighbour[i].status,
               node.neighbour[i].title);
   }
}

void print_log(void)
{
   printf("\n");
   for (uint8_t i = 0; i < EVENT_HISTORY; ++i) {
      if (node.events[i].type != EMPTY) {
         ESP_LOGI("LOG", "%d. " MACSTR, i, MAC2STR(node.events[i].mac_addr));
      }
   }
}

uint8_t get_cout_active_devices(void)
{
   uint8_t ret = 0;
   for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
      if (node.neighbour[i].status == ACTIVE)
         ++ret;
   }
   return ret;
}

static void IRAM_ATTR gpio_handler_isr(void *)
{
   log_event_t data;
   data.timestamp = get_time();
   data.type = PUSH;
   data.task = SEND2MASTER;
   ESP_ERROR_CHECK(esp_read_mac(&data.mac_addr, ESP_MAC_BASE));
   xQueueSendFromISR(isr_event, &data, NULL);
}

void handle_isr_event_task(void)
{
   // wait to button releases
   // to prevent rebounced and get time of push
   log_event_t data;
   while (xQueueReceive(isr_event, &data, portMAX_DELAY) == pdTRUE) {
      uint64_t time = 0;
      while (gpio_get_level(GPIO_NUM_21)) {
         vTaskDelay(10 / portTICK_PERIOD_MS);
         ++time;
      }
      // for long push (grather then 5s) reset log
      if (time > 500)
         data.type = RESET;
      if (xQueueSend(log_event, &data, ESPNOW_MAXDELAY) != pdTRUE)
         ESP_LOGW(TAG, "Send log event into the queue fail");

      // remove bounced interrupts
      while (xQueueReceive(isr_event, &data, CLEANING_DELAY) == pdTRUE)
         ;
   }
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
                      neighbour_t *neighbour, log_event_t *event)
{
   message_data_t *buf = (message_data_t *)data;

   if (data_len < sizeof(message_data_t)) {
      ESP_LOGE(TAG, "Receive ESPNOW data is too short, len:%d", data_len);
      return -1;
   }

   // Administration variables
   *type = buf->type;
   *content = buf->content;
   *epoch_id = buf->epoch_id;

   // DS event varibales
   // *event->mac_addr = buf->event_mac_addr;
   memcpy(event->mac_addr, buf->event_mac_addr, ESP_NOW_ETH_ALEN);
   event->task = buf->event_task;
   event->type = buf->event_type;
   event->timestamp = buf->content;

   // Neighbor list
   for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
      neighbour[i] = buf->neighbour[i];
   }
   return buf->type;
}

void espnow_data_prepare(espnow_send_param_t *send_param)
{
   message_data_t *buf = (message_data_t *)send_param->buf;

   send_param->data_len = sizeof(message_data_t);
   buf->type = send_param->type;
   buf->epoch_id = send_param->epoch_id;
   buf->content = send_param->content;
   buf->event_task = send_param->event_task;
   buf->event_type = send_param->event_type;
   memcpy(buf->event_mac_addr, send_param->event_mac_addr, ESP_NOW_ETH_ALEN);
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
   log_event_t event;
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
         if (send_cb->status != ESP_OK) {
            ESP_LOGW(TAG, "Send to " MACSTR " failed",
                     MAC2STR(send_cb->mac_addr));
            // increase inactive status
            for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
               if (memcmp(send_cb->mac_addr, &node.neighbour[i].mac_addr,
                          ESP_NOW_ETH_ALEN) == 0) {
                  ++node.neighbour_error_count[i];
                  // if device is inactive
                  if (node.neighbour_error_count[i] >=
                      COUNT_ERROR_MESSAGE_TO_INACTIVE) {
                     // mark as inactive
                     node.neighbour[i].status = INACTIVE;
                     // send this infromation into all DS
                     send_param->type = NEIGHBOURS;
                     memcpy(&send_param->neighbour[0], &node.neighbour[0],
                            sizeof(neighbour_t) * NEIGHBOURS_COUNT);
                     for (uint8_t j = 0; j < NEIGHBOURS_COUNT; ++j) {
                        if (node.neighbour[j].status == ACTIVE) {
                           send_param->epoch_id = node.epoch_id;
                           memcpy(send_param->dest_mac,
                                  &node.neighbour[j].mac_addr,
                                  ESP_NOW_ETH_ALEN);
                           espnow_data_prepare(send_param);

                           ret = esp_now_send(send_param->dest_mac,
                                              send_param->buf,
                                              send_param->data_len);
                           if (ret != ESP_OK)
                              handle_espnow_send_error(ret);
                        }
                     }
                  }
               }
            }
         } else {
            for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
               if (memcmp(send_cb->mac_addr, &node.neighbour[i].mac_addr,
                          ESP_NOW_ETH_ALEN) == 0) {
                  node.neighbour_error_count[i] = 0;
               }
            }
         }
         break;
      }
      case ESPNOW_RECV_CB: {
         // handle recv
         espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;
         ret = espnow_data_parse(recv_cb->data, recv_cb->data_len, &type,
                                 &content, &epoch_id, &neighbours, &event);
         free(recv_cb->data);
         if (epoch_id < node.epoch_id)
            ESP_LOGE(TAG, "Wrong number of epoch ID in income message");
         else if (epoch_id > node.epoch_id)
            node.epoch_id = epoch_id;
         switch (ret) {
         case HELLO_DS: {
            // ESP_LOGI(TAG, "Receive HELLO_DS");
            // add device to my list or make it ACTIVE
            if (!esp_now_is_peer_exist(recv_cb->mac_addr)) {
               uint8_t i = 0;
               while (node.neighbour[i].status != NOT_INITIALIZED) {
                  ++i;
                  if (i >= NEIGHBOURS_COUNT) {
                     ESP_LOGE(TAG, "Not empty space for more neighbours");
                  }
               }
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
            // print_neighbours();
            // send back: neighbour list, epoch id
            // ESP_LOGI(TAG, "Send NEIGHBOUR message");
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
            // ESP_LOGI(TAG, "Receive NEIGHBOUR message");
            // save neighbours from peer_list
            for (uint8_t j = 0; j < NEIGHBOURS_COUNT; ++j) {
               if (neighbours[j].status != NOT_INITIALIZED) {
                  if (!is_device_mac(&neighbours[j].mac_addr)) {
                     if (!esp_now_is_peer_exist(&neighbours[j].mac_addr)) {
                        uint8_t i = 0;
                        while (node.neighbour[i].status != NOT_INITIALIZED) {
                           ++i;
                           if (i >= NEIGHBOURS_COUNT) {
                              ESP_LOGE(TAG,
                                       "Not empty space for more neighbours");
                           }
                        }
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
                              // ESP_LOGI(TAG, "Set %d. neighbour as INACTIVE",
                              // i);
                              break;
                           }
                        }
                     }
                  }
               }
            }
            // save sender peer_info
            if (!esp_now_is_peer_exist(recv_cb->mac_addr)) {
               uint8_t i = 0;
               while (node.neighbour[i].status != NOT_INITIALIZED) {
                  ++i;
                  if (i >= NEIGHBOURS_COUNT) {
                     ESP_LOGE(TAG, "Not empty space for more neighbours");
                  }
               }
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
            // print_neighbours();
         } break;
         case RTT_CAL_MASTER: {
            // ESP_LOGI(TAG, "Receive RTT_CAL_MASTER");
            // send value back to master with type RTT_CAL_SLAVE
            send_param->content = content;
            send_param->epoch_id = node.epoch_id;
            send_param->type = RTT_CAL_SLAVE;
            memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
            espnow_data_prepare(send_param);
            if (esp_now_send(send_param->dest_mac, send_param->buf,
                             send_param->data_len) != ESP_OK) {
               ESP_LOGW(TAG, "Send RTT_CAL_MASTER error");
            }
            // ESP_LOGI(TAG, "Send RTT_CAL_SLAVE");
         } break;
         case RTT_CAL_SLAVE: {
            // calcule RTT and send it back to slave with type RTT_VAL
            // ESP_LOGI(TAG, "Receive RTT_CAL_SALVE");
            send_param->content = (evt.timestamp - content) / 2;
            send_param->type = RTT;
            send_param->epoch_id = node.epoch_id;
            memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
            espnow_data_prepare(send_param);
            if (esp_now_send(send_param->dest_mac, send_param->buf,
                             send_param->data_len) != ESP_OK) {
               ESP_LOGW(TAG, "Send RTT_CAL_SLAVE error");
            }
            // ESP_LOGI(TAG, "Send RTT");
         } break;
         case RTT: {
            // set RTT value into the array
            // ESP_LOGI(TAG, "Receive RTT");
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
         } break;
         case TIME: {
            // calcule deviation O~
            node.deviation_avg = (int32_t)get_time_with_timer(evt.timestamp) -
                                 (int32_t)content - (int32_t)get_rtt_avg();

            if (node.deviation_avg < DEVIATION_LIMIT &&
                node.deviation_avg > -DEVIATION_LIMIT) {
               node.is_time_synced = 1;
            }

            // set time
            if (node.is_time_synced) {
               if (node.deviation_avg > DEVIATION_MAX_CONSTANT)
                  node.time_corection += DEVIATION_MAX_CONSTANT;
               else if (node.deviation_avg < -DEVIATION_MAX_CONSTANT)
                  node.time_corection -= DEVIATION_MAX_CONSTANT;
               // ESP_LOGI(TAG, "S %ld", node.deviation_avg);
            } else {
               // ESP_LOGI(TAG, "F %ld", node.deviation_avg);
               node.time_corection =
                   esp_timer_get_time() - (content + (uint64_t)get_rtt_avg());
            }
            // ESP_LOGI(TAG, "Receive TIME");

            // set timestampt to calcule new election
            node.timeout_sync = evt.timestamp;
            node.title = SLAVE;

            // set MASTER node
            for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
               if (memcmp(recv_cb->mac_addr, &node.neighbour[i].mac_addr,
                          ESP_NOW_ETH_ALEN) == 0) {
                  node.neighbour[i].title = MASTER;
               } else {
                  node.neighbour[i].title = SLAVE;
               }
            }
         } break;
         case REQUEST_VOTE: {
            // ESP_LOGI(TAG, "Receive REQUEST_VOTE");
            // send value back to master with type GIVE_VOTE
            send_param->epoch_id = node.epoch_id;
            send_param->type = GIVE_VOTE;
            memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
            espnow_data_prepare(send_param);
            if (esp_now_send(send_param->dest_mac, send_param->buf,
                             send_param->data_len) != ESP_OK) {
               ESP_LOGW(TAG, "Send REQUEST_VOTE error");
            }
            // ESP_LOGI(TAG, "Send GIVE_VOTE");
         } break;
         case GIVE_VOTE: {
            // ESP_LOGI(TAG, "Receive GIVE_VOTE");
            if (node.title == CANDIDATE) {
               ++node.count_of_vote;
               if (node.count_of_vote >=
                   ((get_cout_active_devices() + 1) / 2)) {
                  node.title = MASTER;
                  for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i)
                     node.neighbour[i].title = SLAVE;
               }
            }
         } break;
         case LOG2MASTER: {
            log_event_t data;
            data.timestamp = event.timestamp;
            data.type = event.type;
            memcpy(&data.mac_addr, &event.mac_addr, ESP_NOW_ETH_ALEN);
            data.task = SEND2SLAVES;
            if (xQueueSend(log_event, &data, DS_MAXDELAY) != pdTRUE) {
               ESP_LOGE(TAG, "Can't push data into the log_event");
            };
         } break;
         case LOG2SLAVES: {
            log_event_t data;
            data.timestamp = event.timestamp;
            data.type = event.type;
            memcpy(&data.mac_addr, &event.mac_addr, ESP_NOW_ETH_ALEN);
            data.task = SAVE;
            if (xQueueSend(log_event, &data, DS_MAXDELAY) != pdTRUE) {
               ESP_LOGE(TAG, "Can't push data into the log_event");
            };
         } break;
         default:
            ESP_LOGE(TAG, "Receive unknown message type, %d message_type_t",
                     ret);
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
   // ESP_LOGI(TAG, "Send HELLO_DS");
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

void send_rtt_cal_master_task(void)
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
   send_param->type = RTT_CAL_MASTER;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vTaskDelete(NULL);
   }
   esp_err_t ret;
   while (1) {
      if (node.title == MASTER) {
         // ESP_LOGI(TAG, "Send RTT_CAL_MASTER");
         for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
            if (node.neighbour[i].status == ACTIVE) {
               memcpy(send_param->dest_mac, &node.neighbour[i].mac_addr,
                      ESP_NOW_ETH_ALEN);
               send_param->epoch_id = node.epoch_id;
               send_param->content = esp_timer_get_time();
               espnow_data_prepare(send_param);

               ret = esp_now_send(send_param->dest_mac, send_param->buf,
                                  send_param->data_len);
               if (ret != ESP_OK)
                  handle_espnow_send_error(ret);
            }
         }
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);
   }
   free(send_param);
   vTaskDelete(NULL);
}

void send_request_vote_task(void)
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
   send_param->type = REQUEST_VOTE;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vTaskDelete(NULL);
   }
   esp_err_t ret;
   while (1) {
      // check timeout sync, longer than 2s, new elecetion
      if (node.title == SLAVE &&
          (esp_timer_get_time() - node.timeout_sync) > MASTER_TIMEOUT) {
         // ESP_LOGI(TAG, "Send REQUEST_VOTE");
         ++node.epoch_id;
         node.count_of_vote = 0;
         node.title = CANDIDATE;
         // send give request vote
         for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
            if (node.neighbour[i].status == ACTIVE) {
               memcpy(send_param->dest_mac, &node.neighbour[i].mac_addr,
                      ESP_NOW_ETH_ALEN);
               send_param->epoch_id = node.epoch_id;
               espnow_data_prepare(send_param);

               ret = esp_now_send(send_param->dest_mac, send_param->buf,
                                  send_param->data_len);
               if (ret != ESP_OK)
                  handle_espnow_send_error(ret);
            }
         }
         node.timeout_vote = esp_timer_get_time();
         while (1) {
            // zařízení dostane potvrzení od většiny GIVE_VOTE aktivních sousedů
            // a stane se novým lídrem nebo přijme zprávu synchronizující čas
            // TIME, novým lídrem se stalo
            if (node.title != CANDIDATE) {
               break;
            }
            // nebo budou volby neúspěšné do timeoutu, volby skončí neúspěchem a
            // začne nová epocha
            if (esp_timer_get_time() - node.timeout_vote > VOTE_TIMEOUT) {
               node.title = SLAVE;
               break;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
         }
      }
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }
   free(send_param);
   vTaskDelete(NULL);
}

void send_time_task(void)
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
   send_param->type = TIME;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vTaskDelete(NULL);
   }
   esp_err_t ret;
   while (1) {
      if (node.title == MASTER) {
         // ESP_LOGI(TAG, "Send TIME");
         for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
            if (node.neighbour[i].status == ACTIVE) {
               memcpy(send_param->dest_mac, &node.neighbour[i].mac_addr,
                      ESP_NOW_ETH_ALEN);
               send_param->epoch_id = node.epoch_id;
               send_param->content = esp_timer_get_time();
               espnow_data_prepare(send_param);

               ret = esp_now_send(send_param->dest_mac, send_param->buf,
                                  send_param->data_len);
               if (ret != ESP_OK)
                  handle_espnow_send_error(ret);
            }
         }
      }
      vTaskDelay(500 / portTICK_PERIOD_MS);
   }
   free(send_param);
   vTaskDelete(NULL);
}

void handle_ds_event_task(void)
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
   // send_param->type = TIME;
   send_param->data_len = CONFIG_ESPNOW_SEND_LEN;
   send_param->buf = malloc(CONFIG_ESPNOW_SEND_LEN);
   if (send_param->buf == NULL) {
      ESP_LOGE(TAG, "Malloc send buffer fail");
      free(send_param);
      vTaskDelete(NULL);
   }
   esp_err_t ret;
   log_event_t data;
   // neni idelani implementace, bude se muset prerovnavat, mam ale taky, dam
   // jen nizkou prioritu
   while (xQueueReceive(log_event, &data, portMAX_DELAY) == pdTRUE) {
      uint8_t i;
      ESP_LOGI(TAG, "EVENT %d " MACSTR, data.task, MAC2STR(data.mac_addr));
      for (i = 0; i < EVENT_HISTORY; ++i) {
         if (node.events[i].type == EMPTY) {
            node.events[i].timestamp = data.timestamp;
            node.events[i].type = data.type;
            memcpy(&node.events[i].mac_addr, &data.mac_addr, ESP_NOW_ETH_ALEN);
            ESP_LOGI(TAG, "FIRST");
            break;
         }
         if (node.events[i].timestamp > data.timestamp) {
            // shift to the right
            ESP_LOGI(TAG, "Shift audio");
            memcpy(&node.events[i + 1], &node.events[i],
                   sizeof(log_event_t) * (EVENT_HISTORY - i - 1));
            // save date
            node.events[i].timestamp = data.timestamp;
            node.events[i].type = data.type;
            memcpy(&node.events[i].mac_addr, &data.mac_addr, ESP_NOW_ETH_ALEN);
            break;
         }
         if ((i + 1) == EVENT_HISTORY)
            ESP_LOGE(TAG, "Log is full");
      }
      // distribute data
      send_param->epoch_id = node.epoch_id;
      memcpy(send_param->event_mac_addr, &data.mac_addr, ESP_NOW_ETH_ALEN);
      send_param->event_type = data.type;
      send_param->content = data.timestamp;
      // ESP_LOGI(TAG, "Mac: " MACSTR " Timestamp %lld Event type %d",
      // MAC2STR(send_param->event_mac_addr), send_param->content,
      // send_param->event_type);
      switch (data.task) {
      case SEND2MASTER:
         // ESP_LOGI(TAG, "SEND2MASTER");
         if (node.title != MASTER) {
            uint8_t i = 0;
            while (node.neighbour[i].title != MASTER) {
               for (i = 0; i < NEIGHBOURS_COUNT; ++i) {
                  if (node.neighbour[i].title == MASTER)
                     break;
               }
               vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            memcpy(send_param->dest_mac, &node.neighbour[i].mac_addr,
                   ESP_NOW_ETH_ALEN);
            send_param->type = LOG2MASTER;
            espnow_data_prepare(send_param);
            ret = esp_now_send(send_param->dest_mac, send_param->buf,
                               send_param->data_len);
            if (ret != ESP_OK)
               handle_espnow_send_error(ret);
            break;
         }
      case SEND2SLAVES:
         // ESP_LOGI(TAG, "SEND2SLAVES");
         send_param->type = LOG2SLAVES;
         for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
            if (node.neighbour[i].status == ACTIVE) {
               // exclude the node source mac address
               if (is_same_mac(&node.neighbour[i].mac_addr, &data.mac_addr))
                  break;
               memcpy(send_param->dest_mac, &node.neighbour[i].mac_addr,
                      ESP_NOW_ETH_ALEN);
               espnow_data_prepare(send_param);

               ret = esp_now_send(send_param->dest_mac, send_param->buf,
                                  send_param->data_len);
               if (ret != ESP_OK)
                  handle_espnow_send_error(ret);
            }
         }
         break;
      case SAVE:
         // do nothing
         break;
      default:
         ESP_LOGE(TAG, "Unknown task type");
         break;
      }
   }
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
   ESP_LOGI(TAG, "Reset");
   ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_21));
   assert(GPIO_IS_VALID_GPIO(GPIO_NUM_21));
   gpio_config_t cfg = {
       .pin_bit_mask = BIT64(GPIO_NUM_21),
       .mode = GPIO_MODE_INPUT,
       .pull_up_en = GPIO_PULLUP_DISABLE,
       .pull_down_en = GPIO_PULLDOWN_ENABLE,
       .intr_type = GPIO_INTR_POSEDGE,
   };
   gpio_config(&cfg);

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
   node.title = SLAVE;
   for (uint8_t i = 0; i < NEIGHBOURS_COUNT; ++i) {
      node.neighbour[i].status = NOT_INITIALIZED;
      node.neighbour_error_count[i] = 0;
   }
   for (uint8_t i = 0; i < EVENT_HISTORY; ++i) {
      node.events[i].type = EMPTY;
   }

   // Create a broadcast peer
   memcpy(&broadcast_peer.peer_addr, s_broadcast_mac, 6);
   if (!esp_now_is_peer_exist(s_broadcast_mac)) {
      ESP_ERROR_CHECK(esp_now_add_peer(&broadcast_peer));
   }

   espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));
   log_event = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(log_event_t));
   isr_event = xQueueCreate(ISR_QUEUE_SIZE, sizeof(log_event_t));

   BaseType_t handler_task;
   handler_task =
       xTaskCreate((TaskFunction_t)espnow_handler_task, "espnow_handler_task",
                   STACK_SIZE, NULL, PRIORITY_HANDLER, NULL);
   BaseType_t send_rtt_cal_master_task_v;
   send_rtt_cal_master_task_v = xTaskCreate(
       (TaskFunction_t)send_rtt_cal_master_task, "send_rtt_cal_master_task",
       STACK_SIZE, NULL, PRIORITY_RTT_START, NULL);

   BaseType_t send_time_task_v;
   send_time_task_v =
       xTaskCreate((TaskFunction_t)send_time_task, "send_time_task", STACK_SIZE,
                   NULL, PRIORITY_TIME_START, NULL);

   BaseType_t send_request_vote_task_v;
   send_request_vote_task_v = xTaskCreate(
       (TaskFunction_t)send_request_vote_task, "send_request_vote_task",
       STACK_SIZE, NULL, PRIORITY_REQUEST_TASK, NULL);

   BaseType_t handle_ds_event_task_v;
   handle_ds_event_task_v =
       xTaskCreate((TaskFunction_t)handle_ds_event_task, "handle_ds_event_task",
                   STACK_SIZE, NULL, PRIORITY_HANDLE_DS_EVENT, NULL);

   BaseType_t handle_isr_event_task_v;
   handle_isr_event_task_v = xTaskCreate((TaskFunction_t)handle_isr_event_task,
                                         "handle_isr_event_task", STACK_SIZE,
                                         NULL, PRIORITY_HANDLE_ISR_EVENT, NULL);

   send_hello_ds_message();

   vTaskDelay(1000 / portTICK_PERIOD_MS);

   while (1) {
      print_neighbours();
      print_log();
      vTaskDelay(5000 / portTICK_PERIOD_MS);
   }

   // Ending rutine
   printf("Restarting now!\n");
   fflush(stdout);
   esp_restart();
}
