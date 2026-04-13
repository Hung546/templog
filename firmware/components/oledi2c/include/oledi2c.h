#ifndef OLED_H
#define OLED_H

#include "driver/i2c.h"

#define OLED_ADD 0x3C

void oled_init(i2c_port_t i2c_num);
void oled_clear(i2c_port_t i2c_num);
void oled_draw_bitmap(i2c_port_t i2c_num, uint8_t x, uint8_t y_page, uint8_t w,
                      uint8_t h, const uint8_t *bitmap);
void oled_draw_string(i2c_port_t i2c_num, uint8_t x, uint8_t y_page,
                      const char *str);
void oled_draw_big_numbers(i2c_port_t i2c_num, uint8_t x, uint8_t y_page,
                           const char *str);
void oled_draw_medium_numbers(i2c_port_t i2c_num, uint8_t x, uint8_t y_page,
                              const char *str);
#endif
