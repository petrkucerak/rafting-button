/**
 * @file peripheral.c
 * @brief Contains functions for basic manipulation with peripherals.
 * @version 1.0
 * @date 2023-06-08
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 *
 * @note This code is subject to the terms of the MIT license.
 *
 */

#include "peripheral.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

void config_led(gpio_num_t gpio_num)
{
   // Set up GPIO direction as output
   gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
}

void turn_on_led(gpio_num_t gpio_num)
{
   // Set GPIO output level to HIGH (1)
   gpio_set_level(gpio_num, 1);
}

void turn_off_led(gpio_num_t gpio_num)
{
   // Set GPIO output level to LOW (0)
   gpio_set_level(gpio_num, 0);
}

void turn_on_buildin_led()
{
   gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
   gpio_set_level(GPIO_NUM_2, 1);
}

void turn_off_buildin_led() { gpio_set_level(GPIO_NUM_2, 0); }

void do_blink_task(uint16_t mils)
{
   config_led(GPIO_NUM_23);
   turn_on_led(GPIO_NUM_23);
   turn_on_buildin_led();

   vTaskDelay(mils / portTICK_PERIOD_MS);

   turn_off_led(GPIO_NUM_23);
   turn_off_buildin_led();

   vTaskDelete(NULL);
}

void do_blink(uint16_t mils)
{
   turn_on_led(GPIO_NUM_23);
   turn_on_buildin_led();

   vTaskDelay(mils / portTICK_PERIOD_MS);

   turn_off_led(GPIO_NUM_23);
   turn_off_buildin_led();
}

void turn_on_led_task(gpio_num_t gpio_num)
{
   turn_on_buildin_led();
   turn_on_led(gpio_num);
   vTaskDelete(NULL);
}

void turn_off_led_task(gpio_num_t gpio_num)
{
   turn_off_buildin_led();
   turn_off_led(gpio_num);
   vTaskDelete(NULL);
}