#ifndef __UTILS_H
#define __UTILS_H

#include <stdio.h>

/**
 * @brief Initialize all pins on ESP32-S2 Dev Board
 *
 */
void all_pins_init();

/**
 * @brief Activate all pins on ESP32-S2 Dev Board
 *
 */
void all_pins_test();

/**
 * @brief The function displays the mac address on the build-in display.
 *
 */
void mac_on_display();


void red_blick(uint16_t time);

#endif