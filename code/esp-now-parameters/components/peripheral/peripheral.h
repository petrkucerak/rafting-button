#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <driver/gpio.h>

/**
 * @brief Configure GPIO for led diode
 *
 * @param gpio_num GPIO number
 */
void config_led(gpio_num_t gpio_num);

/**
 * @brief Turn on GPIO output for LED diode
 *
 * @param gpio_num GPIO number
 */
void turn_on_led(gpio_num_t gpio_num);

/**
 * @brief Turn off GPIO output for LED diode
 *
 * @param gpio_num GPIO number
 */
void turn_off_led(gpio_num_t gpio_num);

/**
 * @brief Configure GPIO and turn on built-in LED diode
 *
 */
void turn_on_buildin_led();

/**
 * @brief Turn off built-in LED diode
 *
 */
void turn_off_buildin_led();

/**
 * @brief Do blick by buildin LED and on GPIO23
 *
 * @param mils time of blick in miliseconds
 */
void do_blick_task(uint16_t mils);

void turn_on_led_task(gpio_num_t gpio_num);

void turn_off_led_task(gpio_num_t gpio_num);

#endif // PERIPHERAL_H