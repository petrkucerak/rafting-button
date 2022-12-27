#include "LCD096.h"
#include "letters.h"

#define VSPI_MISO LCD_MISO_IO
#define VSPI_MOSI LCD_MOSI_IO
#define VSPI_SCLK LCD_SCK_IO
#define VSPI_SS LCD_CS_IO

#define VSPI HSPI
static const int spiClk = 10000000; // 10 MHz
SPIClass *vspi = NULL;

/**
 * @brief The function initializes the built-in display.
 *
 */
void lcd_init(void);

/**
 * @brief The function resets the built-in display.
 *
 */
void lcd_reset(void);

/**
 * @brief The function sets up command at the built-in display.
 *
 * @param cmd [uint8]
 */
void lcd_write_cmd(uint8_t cmd);

/**
 * @brief The function sets up data at the built-in display.
 *
 * @param dat [uint8]
 */
void lcd_write_data(uint8_t dat);

/**
 * @brief The function sets up the writing frame size at the built-in display.
 * Limits for display are: WIDTH - 160, HEIGHT - 80.
 *
 * @param x_start [uint16]
 * @param y_start [uint16]
 * @param x_end [uint16]
 * @param y_end [uint16]
 */
void lcd_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                    uint16_t y_end);

/**
 * @brief The function sets up testing colors on the built-in display.
 *
 */
void lcd_test(void);

/**
 * @brief The function tests the status of the built-in display.
 *
 */
void lcd_border_check(void);

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
void lcd_write_letter(uint32_t letter, uint8_t x, uint8_t y, uint16_t color,
                      uint16_t bg_color, uint8_t font_size);

/**
 * @brief This function fills all display current color.
 *
 * @param color 16-bit color code
 */
void lcd_set_color(uint16_t color);

void lcd_init(void) {
  vspi = new SPIClass(VSPI);
  vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS); // SCLK, MISO, MOSI, SS
  pinMode(VSPI_SS, OUTPUT);                              // VSPI SS
  pinMode(LCD_RESET_IO, OUTPUT);
  pinMode(LCD_BL_IO, OUTPUT);
  pinMode(LCD_DC_IO, OUTPUT);
  digitalWrite(LCD_BL_IO, HIGH);
  digitalWrite(LCD_DC_IO, HIGH);

  lcd_reset();

  lcd_write_cmd(0x11);
  delay(10);
  lcd_write_cmd(0x21);
  lcd_write_cmd(0x21);

  lcd_write_cmd(0xB1);
  lcd_write_data(0x05);
  lcd_write_data(0x3A);
  lcd_write_data(0x3A);

  lcd_write_cmd(0xB2);
  lcd_write_data(0x05);
  lcd_write_data(0x3A);
  lcd_write_data(0x3A);

  lcd_write_cmd(0xB3);
  lcd_write_data(0x05);
  lcd_write_data(0x3A);
  lcd_write_data(0x3A);
  lcd_write_data(0x05);
  lcd_write_data(0x3A);
  lcd_write_data(0x3A);

  lcd_write_cmd(0xB4);
  lcd_write_data(0x03);

  lcd_write_cmd(0xC0);
  lcd_write_data(0x62);
  lcd_write_data(0x02);
  lcd_write_data(0x04);

  lcd_write_cmd(0xC1);
  lcd_write_data(0xC0);

  lcd_write_cmd(0xC2);
  lcd_write_data(0x0D);
  lcd_write_data(0x00);

  lcd_write_cmd(0xC3);
  lcd_write_data(0x8D);
  lcd_write_data(0x6A);

  lcd_write_cmd(0xC4);
  lcd_write_data(0x8D);
  lcd_write_data(0xEE);

  lcd_write_cmd(0xC5);
  lcd_write_data(0x0E);

  lcd_write_cmd(0xE0);
  lcd_write_data(0x10);
  lcd_write_data(0x0E);
  lcd_write_data(0x02);
  lcd_write_data(0x03);
  lcd_write_data(0x0E);
  lcd_write_data(0x07);
  lcd_write_data(0x02);
  lcd_write_data(0x07);
  lcd_write_data(0x0A);
  lcd_write_data(0x12);
  lcd_write_data(0x27);
  lcd_write_data(0x37);
  lcd_write_data(0x00);
  lcd_write_data(0x0D);
  lcd_write_data(0x0E);
  lcd_write_data(0x10);

  lcd_write_cmd(0xE1);
  lcd_write_data(0x10);
  lcd_write_data(0x0E);
  lcd_write_data(0x03);
  lcd_write_data(0x03);
  lcd_write_data(0x0F);
  lcd_write_data(0x06);
  lcd_write_data(0x02);
  lcd_write_data(0x08);
  lcd_write_data(0x0A);
  lcd_write_data(0x13);
  lcd_write_data(0x26);
  lcd_write_data(0x36);
  lcd_write_data(0x00);
  lcd_write_data(0x0D);
  lcd_write_data(0x0E);
  lcd_write_data(0x10);

  lcd_write_cmd(0x3A);
  lcd_write_data(0x05);

  lcd_write_cmd(0x36);
  lcd_write_data(0xA8);

  lcd_write_cmd(0x29);
}

void lcd_reset(void) {
  digitalWrite(LCD_RESET_IO, LOW);
  delay(10);
  digitalWrite(LCD_RESET_IO, HIGH);
  delay(10);
}

void lcd_write_cmd(uint8_t cmd) {
  digitalWrite(LCD_DC_IO, LOW);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  vspi->transfer(cmd);
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
}

void lcd_write_data(uint8_t dat) {
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  vspi->transfer(dat);
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
}

void lcd_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end,
                    uint16_t y_end) {
  if (x_start > 0)
    x_start -= 1;
  if (y_start > 0)
    y_start -= 1;
  if (x_end > 0)
    x_end -= 1;
  if (y_end > 0)
    y_end -= 1;

  x_start += 1;
  x_end += 1;
  y_start += 26;
  y_end += 26;

  lcd_write_cmd(0x2A);
  lcd_write_data(x_start >> 8);
  lcd_write_data(x_start);
  lcd_write_data(x_end >> 8);
  lcd_write_data(x_end);

  lcd_write_cmd(0x2B);
  lcd_write_data(y_start >> 8);
  lcd_write_data(y_start);
  lcd_write_data(y_end >> 8);
  lcd_write_data(y_end);
  lcd_write_cmd(0x2C);
}

void lcd_test(void) {
  lcd_set_window(0, 0, LCD_WIDTH, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
    vspi->transfer16(0xf800);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);

  lcd_set_window(0, 0, LCD_WIDTH, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
    vspi->transfer16(0x07e0);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);

  lcd_set_window(0, 0, LCD_WIDTH, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
    vspi->transfer16(0x001f);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);
}

void lcd_border_check(void) {
  lcd_set_window(0, 0, LCD_WIDTH, 0);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_WIDTH; i++) {
    vspi->transfer16(0xFFFF);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);

  lcd_set_window(0, 0, 0, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_HEIGHT; i++) {
    vspi->transfer16(0xFFFF);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);

  lcd_set_window(0, LCD_HEIGHT, LCD_WIDTH, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_WIDTH; i++) {
    vspi->transfer16(0xFFFF);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);

  lcd_set_window(LCD_WIDTH, 0, LCD_WIDTH, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_HEIGHT; i++) {
    vspi->transfer16(0xFFFF);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  delay(1500);
}

void lcd_set_color(uint16_t color) {
  lcd_set_window(0, 0, LCD_WIDTH, LCD_HEIGHT);
  digitalWrite(LCD_DC_IO, HIGH);
  digitalWrite(LCD_CS_IO, LOW);
  vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (unsigned long i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
    vspi->transfer16(color);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
  // delay(1);
}

void lcd_write_letter(uint32_t letter, uint8_t x, uint8_t y, uint16_t color,
                      uint16_t bg_color, uint8_t font_size) {

  if (font_size == 24) {
    lcd_set_window(x, y, x + 23, y + 23);
    digitalWrite(LCD_DC_IO, HIGH);
    digitalWrite(LCD_CS_IO, LOW);
    vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));

    for (int i = 0; i < 3 * 24; ++i) {
      for (int j = 0; j < 8; ++j) {
        if ((letters24[ASCII2LETTERS24(letter) + i] & (1 << (8 - j - 1))) !=
            0) // lettersx are in letters.h
          vspi->transfer16(color);
        else
          vspi->transfer16(bg_color);
      }
    }
  } else if (font_size == 20) {
    lcd_set_window(x, y, x + 15, y + 19);
    digitalWrite(LCD_DC_IO, HIGH);
    digitalWrite(LCD_CS_IO, LOW);
    vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));

    for (int i = 0; i < 2 * 20; ++i) {
      for (int j = 0; j < 8; ++j) {
        if ((letters20[ASCII2LETTERS20(letter) + i] & (1 << (8 - j - 1))) !=
            0) // lettersx are in letters.h
          vspi->transfer16(color);
        else
          vspi->transfer16(bg_color);
      }
    }
  } else if (font_size == 16) {
    lcd_set_window(x, y, x + 15, y + 15);
    digitalWrite(LCD_DC_IO, HIGH);
    digitalWrite(LCD_CS_IO, LOW);
    vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));

    for (int i = 0; i < 2 * 16; ++i) {
      for (int j = 0; j < 8; ++j) {
        if ((letters16[ASCII2LETTERS16(letter) + i] & (1 << (8 - j - 1))) !=
            0) // lettersx are in letters.h
          vspi->transfer16(color);
        else
          vspi->transfer16(bg_color);
      }
    }
  } else if (font_size == 12) {
    lcd_set_window(x, y, x + 7, y + 11);
    digitalWrite(LCD_DC_IO, HIGH);
    digitalWrite(LCD_CS_IO, LOW);
    vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));

    for (int i = 0; i < 1 * 12; ++i) {
      for (int j = 0; j < 8; ++j) {
        if ((letters12[ASCII2LETTERS12(letter) + i] & (1 << (8 - j - 1))) !=
            0) // lettersx are in letters.h
          vspi->transfer16(color);
        else
          vspi->transfer16(bg_color);
      }
    }
  } else if (font_size == 8) {
    lcd_set_window(x, y, x + 7, y + 7);
    digitalWrite(LCD_DC_IO, HIGH);
    digitalWrite(LCD_CS_IO, LOW);
    vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));

    for (int i = 0; i < 1 * 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if ((letters8[ASCII2LETTERS8(letter) + i] & (1 << (8 - j - 1))) !=
            0) // lettersx are in letters.h
          vspi->transfer16(color);
        else
          vspi->transfer16(bg_color);
      }
    }
  } else {
    fprintf(stderr, "ERROR: unknown font size!\n");
    exit(1);
  }
  vspi->endTransaction();
  digitalWrite(LCD_CS_IO, HIGH);
}

lcd_s lcd_dev = {lcd_init,         lcd_reset,      lcd_write_cmd,
                 lcd_write_data,   lcd_set_window, lcd_test,
                 lcd_border_check, lcd_set_color,  lcd_write_letter};
