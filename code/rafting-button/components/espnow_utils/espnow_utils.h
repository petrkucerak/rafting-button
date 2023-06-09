/**
 * @file espnow_utils.h
 * @brief Contains functions for working with the ESP-NOW protocol.
 * @details This file provides functions for initializing and deinitializing
 * ESP-NOW, initializing Wi-Fi for ESP-NOW, printing the device's MAC address,
 * and comparing MAC addresses.
 * @version 1.0
 * @date 2023-06-08
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 *
 * @note This code is subject to the terms of the MIT license.
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
 * @brief Initializes Wi-Fi for ESP-NOW.
 */
void wifi_init(void);

/**
 * @brief Initializes ESP-NOW.
 * @return esp_err_t The result of the initialization.
 */
esp_err_t custom_espnow_init(void);

/**
 * @brief Deinitializes ESP-NOW.
 * @return esp_err_t The result of the deinitialization.
 */
esp_err_t custom_espnow_deinit(void);

/**
 * @brief Prints the MAC address of the device.
 */
void print_mac_address();

/**
 * @brief Compares the device's MAC address with the provided address.
 * @param mac_addr The MAC address to compare with the device's address.
 * @return uint8_t Logic value indicating if the addresses match (1 - true, 0 -
 * false).
 */
uint8_t is_device_mac(uint8_t *mac_addr);

/**
 * @brief Compares two MAC addresses.
 * @param mac_1 The first MAC address to compare.
 * @param mac_2 The second MAC address to compare.
 * @return uint8_t Logic value indicating if the addresses match (1 - true, 0 -
 * false).
 */
uint8_t is_same_mac(uint8_t *mac_1, uint8_t *mac_2);

#endif // ESPNOW_UTILS_H
