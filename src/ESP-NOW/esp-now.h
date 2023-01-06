#ifndef __ESPNOW_H
#define __ESPNOW_H

#include "USB-C/USB-C.h"
#include <esp_now.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef struct ESP_NOW_HANDLER_S {
   uint8_t sender_mac_addr[6];
   uint8_t isEmpty;
   char data[250];
   uint8_t data_len;
} ESP_NOW_HANDLER;

typedef struct {
   void (*sent_callback)(const uint8_t *mac_addr, esp_now_send_status_t status);
   void (*receive_callback)(const uint8_t *mac_addr, const uint8_t *data,
                            int data_len);
} esp_now_s;

/**
 * @brief Function for formating MAC Address to classical hexa form.
 *
 * @param mac_address origin
 * @param buffer target
 * @param max_size typical 18
 */
void format_mac_address(const uint8_t *mac_address, char *buffer, int max_size);

/**
 * @brief The goal of this function is send back receive message.
 * The detail description of this function:
 *    1. check if `esp_now_handler` is an empty (!isEmpty),
 *    2. init a new peer connection,
 *    3. copy the target mac address into a peer structure,
 *    4. check if peer connecion with this current mac address doesn't exist,
 *    5. if peer connection doesn't exist connect,
 *    6. then sent a message back,
 *    7. set `esp_now_handler` as empty (isEmpty = TRUE).
 * TODO: mutexes?
 */
void esp_now_echo();

/**
 * @brief The goal of this function is measure latency and print it in specific
 * format to the USB serial output.
 *
 */
void esp_now_test_latency(uint16_t message_count, uint8_t message_size,
                          uint8_t *mac_address);

extern esp_now_s esp_now_dev;
extern ESP_NOW_HANDLER esp_now_handler;

#endif // __ESPNOW_H