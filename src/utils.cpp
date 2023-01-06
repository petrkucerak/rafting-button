#include "utils.h"
#include "board.h"
#include "display/16bit-colors.h"
#include "display/LCD096.h"
#include "display/letters.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

void all_pins_init()
{
   pinMode(TEST_GPIO2, OUTPUT);
   pinMode(TEST_GPIO3, OUTPUT);
   pinMode(TEST_GPIO4, OUTPUT);
   pinMode(TEST_GPIO5, OUTPUT);
   pinMode(TEST_GPIO6, OUTPUT);
   pinMode(TEST_GPIO7, OUTPUT);
   pinMode(TEST_GPIO8, OUTPUT);

   pinMode(TEST_GPIO10, OUTPUT);
   pinMode(TEST_GPIO11, OUTPUT);
   pinMode(TEST_GPIO12, OUTPUT);
   pinMode(TEST_GPIO13, OUTPUT);
   pinMode(TEST_GPIO14, OUTPUT);
   pinMode(TEST_GPIO15, OUTPUT);
   pinMode(TEST_GPIO16, OUTPUT);
   pinMode(TEST_GPIO17, OUTPUT);

   pinMode(TEST_GPIO34, OUTPUT);
   pinMode(TEST_GPIO35, OUTPUT);
   pinMode(TEST_GPIO36, OUTPUT);
   pinMode(TEST_GPIO37, OUTPUT);
   pinMode(TEST_GPIO38, OUTPUT);
   pinMode(TEST_GPIO39, OUTPUT);
   pinMode(TEST_GPIO40, OUTPUT);
   pinMode(TEST_GPIO41, OUTPUT);
   pinMode(TEST_GPIO42, OUTPUT);
   pinMode(TEST_GPIO43, OUTPUT);
   pinMode(TEST_GPIO44, OUTPUT);
}

void all_pins_test()
{
   digitalWrite(TEST_GPIO2, HIGH);
   digitalWrite(TEST_GPIO3, HIGH);
   digitalWrite(TEST_GPIO4, HIGH);
   digitalWrite(TEST_GPIO5, HIGH);
   digitalWrite(TEST_GPIO6, HIGH);
   digitalWrite(TEST_GPIO7, HIGH);
   digitalWrite(TEST_GPIO8, HIGH);

   digitalWrite(TEST_GPIO10, HIGH);
   digitalWrite(TEST_GPIO11, HIGH);
   digitalWrite(TEST_GPIO12, HIGH);
   digitalWrite(TEST_GPIO13, HIGH);
   digitalWrite(TEST_GPIO14, HIGH);
   digitalWrite(TEST_GPIO15, HIGH);
   digitalWrite(TEST_GPIO16, HIGH);
   digitalWrite(TEST_GPIO17, HIGH);

   digitalWrite(TEST_GPIO34, HIGH);
   digitalWrite(TEST_GPIO35, HIGH);
   digitalWrite(TEST_GPIO36, HIGH);
   digitalWrite(TEST_GPIO37, HIGH);
   digitalWrite(TEST_GPIO38, HIGH);
   digitalWrite(TEST_GPIO39, HIGH);
   digitalWrite(TEST_GPIO40, HIGH);
   digitalWrite(TEST_GPIO41, HIGH);
   digitalWrite(TEST_GPIO42, HIGH);
   digitalWrite(TEST_GPIO43, HIGH);
   digitalWrite(TEST_GPIO44, HIGH);

   delay(1000);

   digitalWrite(TEST_GPIO2, LOW);
   digitalWrite(TEST_GPIO3, LOW);
   digitalWrite(TEST_GPIO4, LOW);
   digitalWrite(TEST_GPIO5, LOW);
   digitalWrite(TEST_GPIO6, LOW);
   digitalWrite(TEST_GPIO7, LOW);
   digitalWrite(TEST_GPIO8, LOW);

   digitalWrite(TEST_GPIO10, LOW);
   digitalWrite(TEST_GPIO11, LOW);
   digitalWrite(TEST_GPIO12, LOW);
   digitalWrite(TEST_GPIO13, LOW);
   digitalWrite(TEST_GPIO14, LOW);
   digitalWrite(TEST_GPIO15, LOW);
   digitalWrite(TEST_GPIO16, LOW);
   digitalWrite(TEST_GPIO17, LOW);

   digitalWrite(TEST_GPIO34, LOW);
   digitalWrite(TEST_GPIO35, LOW);
   digitalWrite(TEST_GPIO36, LOW);
   digitalWrite(TEST_GPIO37, LOW);
   digitalWrite(TEST_GPIO38, LOW);
   digitalWrite(TEST_GPIO39, LOW);
   digitalWrite(TEST_GPIO40, LOW);
   digitalWrite(TEST_GPIO41, LOW);
   digitalWrite(TEST_GPIO42, LOW);
   digitalWrite(TEST_GPIO43, LOW);
   digitalWrite(TEST_GPIO44, LOW);

   delay(2000);
}

void mac_on_display()
{
   // get the mac address
   WiFi.mode(WIFI_STA);
   uint8_t mac[6];
   char macAddress[18] = {0};
   esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac);
   sprintf(macAddress, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);

   // init LCD display
   lcd_dev.lcd_init();
   lcd_dev.lcd_set_color(COLOR_BLACK);
   lcd_dev.lcd_write_letter((uint8_t)'M', 1, 1, COLOR_YELLOW, COLOR_BLACK, 12);
   lcd_dev.lcd_write_letter((uint8_t)'A', 1 + 7, 1, COLOR_YELLOW, COLOR_BLACK,
                            12);
   lcd_dev.lcd_write_letter((uint8_t)'C', 1 + 14, 1, COLOR_YELLOW, COLOR_BLACK,
                            12);
   lcd_dev.lcd_write_letter((uint8_t)':', 1 + 21, 1, COLOR_YELLOW, COLOR_BLACK,
                            12);
   for (uint8_t i = 0; i < 17; ++i) {
      lcd_dev.lcd_write_letter((uint8_t)macAddress[i], 1 + (i + 5) * 7, 1,
                               COLOR_YELLOW, COLOR_BLACK, 12);
   }
}

void progress_bar_on_display(uint8_t percent)
{
   lcd_dev.lcd_write_letter((uint8_t)'0', 1, 24, COLOR_GREEN, COLOR_BLACK, 12);
   lcd_dev.lcd_write_letter((uint8_t)'%', 1 + 7, 24, COLOR_GREEN, COLOR_BLACK,
                            12);
   lcd_dev.lcd_write_letter((uint8_t)'1', 152 - 21, 24, COLOR_BLUE, COLOR_BLACK,
                            12);
   lcd_dev.lcd_write_letter((uint8_t)'0', 152 - 14, 24, COLOR_BLUE, COLOR_BLACK,
                            12);
   lcd_dev.lcd_write_letter((uint8_t)'0', 152 - 7, 24, COLOR_BLUE, COLOR_BLACK,
                            12);
   lcd_dev.lcd_write_letter((uint8_t)'%', 152, 24, COLOR_BLUE, COLOR_BLACK, 12);

   uint8_t progress = percent * 0.15;
   for (uint8_t i = 0; i < 15 && i < progress; ++i) {
      lcd_dev.lcd_write_letter((uint8_t)'=', 20 + (i * 7), 24, COLOR_WHITE,
                               COLOR_BLACK, 12);
   }
}

void remove_progress_bar_from_display()
{
   lcd_dev.lcd_draw_square(COLOR_BLACK, 0, 24, LCD_WIDTH, 24 + 12);
}

void saving_data_on_display()
{
   char text[] = {'S', 'a', 'v', 'i', 'n', 'g', ' ', 'd', 'a', 't', 'a', '\0'};
   uint8_t i = 0;
   while (text[i] != '\0') {
      lcd_dev.lcd_write_letter((uint8_t)text[i], 1 + (i * 7), 36, COLOR_CYAN,
                               COLOR_BLACK, 12);
   }
}

void remove_saving_data_from_display()
{
   lcd_dev.lcd_draw_square(COLOR_BLACK, 0, 36, LCD_WIDTH, 36 + 12);
}

void red_blick(uint16_t time)
{
   digitalWrite(LED_RED_BUILDIN, LOW);
   delay(time);
   digitalWrite(LED_RED_BUILDIN, HIGH);
}