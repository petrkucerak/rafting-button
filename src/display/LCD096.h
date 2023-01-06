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
   /**
    * @brief The function initializes the built-in display.
    *
    */
   void (*lcd_init)(void);

   /**
    * @brief The function resets the built-in display.
    *
    */
   void (*lcd_reset)(void);

   /**
    * @brief The function sets up command at the built-in display.
    *
    * @param cmd [uint8]
    */
   void (*lcd_write_cmd)(uint8_t cmd);

   /**
    * @brief The function sets up data at the built-in display.
    *
    * @param dat [uint8]
    */
   void (*lcd_write_data)(uint8_t dat);

   /**
    * @brief The function sets up the writing frame size at the built-in
    * display. Limits for display are: WIDTH - 160, HEIGHT - 80.
    *
    * @param x_start [uint16]
    * @param y_start [uint16]
    * @param x_end [uint16]
    * @param y_end [uint16]
    */
   void (*lcd_set_window)(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                          uint16_t y_end);

   /**
    * @brief The function sets up testing colors on the built-in display.
    *
    */
   void (*lcd_test)(void);

   /**
    * @brief The function tests the status of the built-in display.
    *
    */
   void (*lcd_border_check)(void);

   /**
    * @brief This function fills all display current color.
    *
    * @param color 16-bit color code
    */
   void (*lcd_set_color)(uint16_t color);

   /**
    * @brief Set letter to the current location
    *
    * @param letter letter in ascii code
    * @param x start x coord
    * @param y start y coord
    * @param color 16-bit color code for font
    * @param bg_color 16-bit color code for background
    * @param font_size select a font size (8, 12, 16, 20, 24)
    */
   void (*lcd_write_letter)(uint32_t letter, uint8_t x, uint8_t y,
                            uint16_t color, uint16_t bg_color,
                            uint8_t font_size);

   /**
    * @brief This function set the color to the defined square.
    *
    * @param color 16-bit color code
    * @param x_start start x coord
    * @param y_start start x coord
    * @param x_end end x coord
    * @param y_end end y coord
    */
   void (*lcd_draw_square)(uint16_t color, uint8_t x_start, uint8_t y_start,
                           uint8_t x_end, uint8_t y_end);
} lcd_s;

extern lcd_s lcd_dev;

#endif
