#include "peripheral.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

void config_led(gpio_num_t gpio_num)
{
   // Set up mode
   gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
   printf("GPIO: config led\n");
}

void turn_on_led(gpio_num_t gpio_num)
{
   // Turn on GPIO output
   gpio_set_level(gpio_num, 1);
   printf("GPIO: turn on\n");
}
void turn_off_led(gpio_num_t gpio_num)
{
   // Turn off GPIO output
   gpio_set_level(gpio_num, 0);
   printf("GPIO: turn off\n");
}

void turn_on_buildin_led()
{
   gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
   gpio_set_level(GPIO_NUM_2, 1);
}
void turn_off_buildin_led() { gpio_set_level(GPIO_NUM_2, 0); }

void do_blick(uint16_t mils)
{
   config_led(GPIO_NUM_23);
   turn_on_led(GPIO_NUM_23);
   turn_on_buildin_led();
   vTaskDelay(mils / portTICK_PERIOD_MS);
   turn_off_led(GPIO_NUM_23);
   turn_off_buildin_led();
}