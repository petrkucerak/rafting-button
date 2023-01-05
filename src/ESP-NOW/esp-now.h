#ifndef __ESPNOW_H
#define __ESPNOW_H

#include <stdio.h>

typedef struct testing_message_s {
   uint8_t id;
   int64_t time;
   char mac_address[18];
} Testing_message;

/**
 * @brief Function for formating MAC Address to classical hexa form.
 *
 * @param mac_address origin
 * @param buffer target
 * @param max_size typical 18
 */
void format_mac_address(const uint8_t *mac_address, char *buffer, int max_size);

void test_esp_now(uint8_t count_of_neighbours);

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
void esp_now_test_latency(uint64_t message_count, uint8_t message_size,
                          uint8_t *mac_address);

#endif