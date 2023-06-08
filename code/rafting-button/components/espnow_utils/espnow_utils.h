/**
 * @file espnow_utils.h
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 * @brief The file contains supported functions for the ESP-NOW protocol.
 * @version 1.0
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ESPNOW_UTILS_H
#define ESPNOW_UTILS_H

#include "peripheral.h"
#include <esp_now.h>
#include <esp_types.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @brief Initialize Wi-Fi for ESP-NOW.
 *
 */
void wifi_init(void);

/**
 * @brief Initialize ESP-NOW.
 *
 * @return esp_err_t
 */
esp_err_t custom_espnow_init(void);

/**
 * @brief Deinit ESP-NOW.
 *
 * @return esp_err_t
 */
esp_err_t custom_espnow_deinit(void);

/**
 * @brief Print the mac address of the device.
 *
 */
void print_mac_address();

/**
 * @brief Compare the device's mac address with an address in the function
 * argument.
 *
 * @param mac_addr mac address to compare with the device address
 * @return uint8_t logic value (1 - true, 0 - false)
 */
uint8_t is_device_mac(uint8_t *mac_addr);

/**
 * @brief Compare two mac addresses.
 *
 * @param mac_1 second mac address to compare
 * @param mac_2 second mac address to comapre
 * @return uint8_t logic value (1 - true, 0 - false)
 */
uint8_t is_same_mac(uint8_t *mac_1, uint8_t *mac_2);

#endif // ESPNOW_UTILS_H