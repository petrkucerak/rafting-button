#include "espnow_utils.h"
#include "peripheral.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define TRUE 1
#define FALSE 0

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
   printf("Recieve callback\n");
}

esp_err_t custom_espnow_init(void)
{

   // Initialzie ESP-NOW
   ESP_ERROR_CHECK(esp_now_init());

   // Init structure for incoming messages
   recv_messages = xQueueCreate(RECV_MESSAGES_COUNT, sizeof(espnow_message_t));
   if (recv_messages == NULL) {
      printf("ERROR: Can't create freeRTOS Queue\n");
      return 1;
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