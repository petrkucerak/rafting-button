#ifndef __LCD096_H
#define __LCD096_H

#include "Arduino.h"
#include <SPI.h>

#define LCD_RESET_IO 21
#define LCD_BL_IO 45
#define LCD_DC_IO 18
#define LCD_CS_IO 9
#define LCD_MOSI_IO 11
#define LCD_MISO_IO 12
#define LCD_SCK_IO 10

#define LCD_WIDTH 160
#define LCD_HEIGHT 80

typedef struct {
  void (*lcd_init)(void);
  void (*lcd_reset)(void);
  void (*lcd_write_cmd)(uint8_t cmd);
  void (*lcd_write_data)(uint8_t dat);
  void (*lcd_set_window)(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                         uint16_t y_end);
  void (*lcd_test)(void);
  void (*lcd_border_check)(void);
  void (*lcd_set_color)(uint16_t color);
} lcd_s;

extern lcd_s lcd_dev;

#endif
