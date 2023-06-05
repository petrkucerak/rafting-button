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

void print_mac_address();
uint8_t is_device_mac(uint8_t *mac_addr);
uint8_t is_same_mac(uint8_t *mac_1, uint8_t *mac_2);

#endif // ESPNOW_UTILS_H