#ifndef __ESPNOW_H
#define __ESPNOW_H

#include <stdio.h>

/**
 * @brief Function for formating MAC Address to classical hexa form.
 *
 * @param mac_address origin
 * @param buffer target
 * @param max_size typical 18
 */
void format_mac_address(const uint8_t *mac_address, char *buffer, int max_size);

#endif