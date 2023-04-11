// The file describes the function of ESP-NOW protocols and defines custom
// structures provided to ESP-NOW.

#ifndef ESPNOW_UTILS_H
#define ESPNOW_UTILS_H

#include "distributed_protocols.h"
#include "peripheral.h"
#include <esp_now.h>
#include <esp_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#define RECV_MESSAGES_COUNT 20
#define ESPNOW_MAXDELAY                                                        \
   512 // value from example
       // https://github.com/espressif/esp-idf/blob/42261df71f12b0b995774d103ce68e40e8fba529/examples/wifi/espnow/main/espnow_example_main.c#L33

typedef struct espnow_message {
   uint8_t mac_addr[6]; // MAC address of sender from esp_now_peer_info_t
   uint8_t data[250];   // DS message
   uint8_t lenght;      // length of message
} espnow_message_t;

static QueueHandle_t recv_messages;

/**
 * @brief Initialize Wi-Fi for ESP-NOW
 *
 */
void wifi_init(void);

/**
 * @brief Initialize ESP-NOW
 *
 * @return esp_err_t
 */
esp_err_t custom_espnow_init(void);

esp_err_t custom_espnow_deinit(void);

espnow_message_t *get_last_message(void);

#endif // ESPNOW_UTILS_H