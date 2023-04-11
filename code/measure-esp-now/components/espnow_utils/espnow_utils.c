#include "espnow_utils.h"
#include "peripheral.h"
#include <esp_log.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/projdefs.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
static const char *TAG = "espnow_utils";

void wifi_init(void)
{
   ESP_ERROR_CHECK(esp_netif_init()); // init  TCP/IP stack layer

   // create loop for system task like Wi-Fi
   ESP_ERROR_CHECK(esp_event_loop_create_default());

   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
   ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // init Wi-Fi with default conf

   ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

   // Set current WiFi power save type
   ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
   ESP_ERROR_CHECK(esp_wifi_start());
   // ESP_ERROR_CHECK(esp_wifi_set_channel())
}

static void custom_espnow_send_cb(const uint8_t *mac_addr,
                                  esp_now_send_status_t status)
{
   printf("Send callback\n");
}
static void custom_espnow_recv_cb(const esp_now_recv_info_t *esp_now_info,
                                  const uint8_t *data, int data_len)
{
   turn_on_led(GPIO_NUM_23);

   espnow_message_t *recv = NULL;
   recv = (espnow_message_t *)malloc(sizeof(espnow_message_t));
   if (recv == NULL) {
      ESP_LOGE(TAG, "ERROR: Can't allocated the memory");
      return;
   }

   recv->lenght = data_len;
   memcpy(recv->mac_addr, esp_now_info->src_addr, 6);
   memcpy(recv->data, data, data_len);

   if (recv_messages != 0) {
      if (xQueueSend(recv_messages, (void *)recv,
                     (TickType_t)ESPNOW_MAXDELAY) != pdTRUE) {
         ESP_LOGE(TAG, "ERROR: can't add recv message into the Queue");
         return;
      }
   }

   turn_off_led(GPIO_NUM_23);
}

void print_messages(void)
{
   espnow_message_t mes;
   while (xQueueReceive(recv_messages, &mes, (TickType_t)ESPNOW_MAXDELAY) ==
          pdPASS) {
      printf("%s [%d len]\n", mes.data, mes.lenght);
   }
}

esp_err_t custom_espnow_init(void)
{

   // Initialzie ESP-NOW
   ESP_ERROR_CHECK(esp_now_init());

   // Init structure for incoming messages
   recv_messages = xQueueCreate(RECV_MESSAGES_COUNT, sizeof(espnow_message_t));
   if (recv_messages == NULL) {
      ESP_LOGE(TAG, "ERROR: Can't create freeRTOS Queue");
      return ESP_ERR_NO_MEM;
   }

   // register callbacks
   esp_err_t status;
   status = esp_now_register_send_cb(custom_espnow_send_cb);
   ESP_ERROR_CHECK(status);
   status = esp_now_register_recv_cb(custom_espnow_recv_cb);
   ESP_ERROR_CHECK(status);

   return ESP_OK;
}

esp_err_t custom_espnow_deinit(void)
{
   ESP_ERROR_CHECK(esp_now_deinit());

   return ESP_OK;
}