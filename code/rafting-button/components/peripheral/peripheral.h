/**
 * @file peripheral.h
 * @brief Defines functions for basic manipulation with peripherals.
 * @details This file provides functions for configuring and controlling GPIO
 * for LED diodes, including the built-in LED. It also includes functions for
 * blinking LEDs and controlling LEDs as tasks.
 * @version 1.0
 * @date 2023-06-08
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 *
 * @note This code is subject to the terms of the MIT license.
 *
 */

#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <driver/gpio.h>

/**
 * @brief Configures GPIO for an LED diode.
 * @param gpio_num The GPIO number to configure.
 */
void config_led(gpio_num_t gpio_num);

/**
 * @brief Turns on the GPIO output for an LED diode.
 * @param gpio_num The GPIO number of the LED diode.
 */
void turn_on_led(gpio_num_t gpio_num);

/**
 * @brief Turns off the GPIO output for an LED diode.
 * @param gpio_num The GPIO number of the LED diode.
 */
void turn_off_led(gpio_num_t gpio_num);

/**
 * @brief Configures GPIO and turns on the built-in LED diode.
 */
void turn_on_builtin_led();

/**
 * @brief Turns off the built-in LED diode.
 */
void turn_off_builtin_led();

/**
 * @brief Blinks the built-in LED and a specified GPIO as a task.
 * @param mils The duration of each blink in milliseconds.
 */
void do_blink_task(uint16_t mils);

/**
 * @brief Blinks the built-in LED and a specified GPIO.
 * @param mils The duration of each blink in milliseconds.
 */
void do_blink(uint16_t mils);

/**
 * @brief Turns on the built-in LED and an LED connected to the specified GPIO
 * as a task.
 * @param gpio_num The GPIO number of the LED.
 */
void turn_on_led_task(gpio_num_t gpio_num);

/**
 * @brief Turns off the built-in LED and an LED connected to the specified GPIO
 * as a task.
 * @param gpio_num The GPIO number of the LED.
 */
void turn_off_led_task(gpio_num_t gpio_num);

#endif // PERIPHERAL_H
