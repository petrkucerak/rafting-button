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

/**
 * @brief The function displays on the LCD display progress bar.
 *
 * @param percent value in range between 0 to 100 (%)
 */
void progress_bar_on_display(uint8_t percent);

/**
 * @brief The function replaces with black color the progress bar.
 *
 */
void remove_progress_bar_from_display();

/**
 * @brief The funcion displays text "Saving data"
 * 
 */
void saving_data_on_display();

/**
 * @brief The funcion removes text "Saving data"
 * 
 */
void remove_saving_data_from_display();

/**
 * @brief The function does a red led light blink on ESP-S2-PICO without LCD
 * built-in display.
 *
 * @param time defines the length of LED blink
 */
void red_blick(uint16_t time);

#endif