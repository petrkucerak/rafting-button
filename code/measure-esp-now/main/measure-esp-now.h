#ifndef MEASURE_ESP_NOW_H
#define MEASURE_ESP_NOW_H

#include "esp_now.h"

/**
 * @brief Measures esp-now latency by sending broadcast messages and logging
 * round-time trip.
 *
 */
void measure_broadcast_task(void);

/**
 * @brief Initialize Wi-Fi for ESP-NOW
 *
 */
static void wifi_init(void);

/**
 * @brief Initialize ESP-NOW
 *
 * @return esp_err_t
 */
static esp_err_t espnow_init(void);

static esp_err_t espnow_deinit(void);

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

#endif // MEASURE_ESP_NOW_H