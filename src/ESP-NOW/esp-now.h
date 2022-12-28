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

void test_esp_now();

#endif