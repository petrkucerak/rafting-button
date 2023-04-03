#ifndef ESPNOW_UTILS_H
#define ESPNOW_UTILS_H

#include "peripheral.h"
#include <esp_now.h>
#include <esp_types.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

typedef struct {
   uint8_t sender_mac_addr[6];
   uint8_t is_empty;
   char data[250];
   uint8_t data_lenght;
} espnow_data_t;

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

/**
 * @brief     Callback function of sending ESPNOW data
 * @param     mac_addr peer MAC address
 * @param     status status of sending ESPNOW data (succeed or fail)
 */
static void espnow_send_cb(const uint8_t *mac_addr,
                           esp_now_send_status_t status);

/**
 * @brief     Callback function of receiving ESPNOW data
 * @param     esp_now_info received ESPNOW packet information
 * @param     data received data
 * @param     data_len length of received data
 * @attention esp_now_info is a local variable，it can only be used in the
 * callback.
 */
static void espnow_recv_cb(const esp_now_recv_info_t *esp_now_info,
                           const uint8_t *data, int data_len);

static void blick_espnow_send_cb(const uint8_t *mac_addr,
                                 esp_now_send_status_t status)
{
   BaseType_t blick_task;
   blick_task = xTaskCreate((void *)do_blick_task, "t_do_blick_send", 4096,
                            (void *)50, tskIDLE_PRIORITY, NULL);
}

static void blick_espnow_recv_cb(const esp_now_recv_info_t *esp_now_info,
                                 const uint8_t *data, int data_len)
{
   BaseType_t blick_task;
   blick_task = xTaskCreate((void *)do_blick_task, "t_do_blick_recv", 4096,
                            (void *)1000, tskIDLE_PRIORITY, NULL);
}

#endif // ESPNOW_UTILS_H