#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "oledi2c.h"
#include "sht30.h"
#include <stdio.h>

// ==============================================================================
// CAU HINH CHAN I2C
// ==============================================================================
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

// ==============================================================================
// KHU VUC BITMAP
// ==============================================================================

// 1. Icon Ong nhiet ke (Kich thuoc: 32x32 pixel = 128 byte)
const uint8_t icon_temp_32x32[128] = {
    0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0xfc, 0x06, 0x03, 0x01,
    0x03, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60,
    0x69, 0x69, 0x09, 0x00, 0xff, 0x00, 0x00, 0xe0, 0x00, 0xff, 0xff, 0x00,
    0x00, 0xf8, 0xfc, 0x06, 0x06, 0x06, 0x06, 0x0c, 0x1c, 0x00, 0x00, 0x1c,
    0x16, 0x1e, 0x0c, 0x00, 0x00, 0x00, 0x60, 0x60, 0x69, 0x69, 0x09, 0x00,
    0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x07, 0x1f, 0x18,
    0x10, 0x10, 0x10, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x63, 0xcc, 0xde, 0x97,
    0xdc, 0xc1, 0x77, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 2. Icon Giot nuoc do am (Kich thuoc: 32x32 pixel = 128 byte)
const uint8_t icon_hum_32x32[128] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x3c, 0x1e, 0x0e,
    0x0e, 0x1c, 0x38, 0xf0, 0xe0, 0xc0, 0x80, 0xc0, 0xe0, 0x70, 0x3c, 0x1c,
    0x1c, 0x38, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x80, 0xe0, 0xf8, 0x7c,
    0x1f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x03, 0x0f, 0x1f, 0x7d, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x03, 0xdf, 0xfe, 0xf8, 0x3f, 0xff, 0xf3, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xf0, 0x20,
    0x80, 0xff, 0xff, 0x3f, 0x38, 0x38, 0x18, 0x1c, 0x0e, 0x07, 0x03, 0x00,
    0x00, 0x01, 0x03, 0x07, 0x0e, 0x1c, 0x38, 0x38, 0x70, 0x70, 0x70, 0x60,
    0x60, 0x74, 0x76, 0x37, 0x3b, 0x3d, 0x1c, 0x0f, 0x07, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// DA XOA ICON Do C va % o day de toi uu RAM

// ==============================================================================
// HAM KHOI TAO I2C CUA ESP32
// ==============================================================================
static esp_err_t i2c_master_init(void) {
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };
  esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
  if (err != ESP_OK)
    return err;
  return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

// ==============================================================================
// TASK CHINH: Doc cam bien va cap nhat OLED
// ==============================================================================
void sensor_display_task(void *pvParameters) {
  float temp = 0.0;
  float hum = 0.0;
  char buffer[16];

  // 1. VE GIAO DIEN TINH (STATIC UI) - Chi ve 1 lan
  // --- Nua tren: Nhiet do ---
  oled_draw_string(I2C_MASTER_NUM, 0, 0, "Temperature:"); // Tieu de Page 0
  oled_draw_bitmap(I2C_MASTER_NUM, 96, 0, 32, 32,
                   icon_temp_32x32); // Icon sat mep phai

  // --- Nua duoi: Do am ---
  oled_draw_string(I2C_MASTER_NUM, 0, 4, "Humidity:"); // Tieu de Page 4
  oled_draw_bitmap(I2C_MASTER_NUM, 96, 4, 32, 32,
                   icon_hum_32x32); // Icon sat mep phai

  // 2. VONG LAP CAP NHAT SO DO
  while (1) {
    if (sht30_read(I2C_MASTER_NUM, &temp, &hum) == ESP_OK) {

      // --- Xu ly Nhiet do ---
      // In 2 so thap phan. Toa do (X=0, Y=1) - Thut xuong 1 Page so voi tieu de
      sprintf(buffer, "%5.2f", temp);
      oled_draw_medium_numbers(I2C_MASTER_NUM, 0, 1, buffer);

      // Ve ky hieu do C nho bang font 5x7 tieu chuan o ngay ben canh so
      oled_draw_string(I2C_MASTER_NUM, 75, 1, "o"); // Ky tu do nho o tren
      oled_draw_string(I2C_MASTER_NUM, 82, 2, "C"); // Chu C to o duoi

      // --- Xu ly Do am ---
      // Toa do (X=0, Y=5) - Thut xuong 1 Page so voi tieu de
      sprintf(buffer, "%5.2f", hum);
      oled_draw_medium_numbers(I2C_MASTER_NUM, 0, 5, buffer);

      // Ve ky hieu % nho
      oled_draw_string(I2C_MASTER_NUM, 80, 6, "%");

    } else {
      oled_draw_string(I2C_MASTER_NUM, 0, 1, "Error!     ");
      oled_draw_string(I2C_MASTER_NUM, 0, 5, "Error!     ");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ==============================================================================
// HAM MAIN
// ==============================================================================
void app_main(void) {
  ESP_ERROR_CHECK(i2c_master_init());
  oled_init(I2C_MASTER_NUM);
  oled_clear(I2C_MASTER_NUM);
  xTaskCreate(sensor_display_task, "sensor_task", 4096, NULL, 5, NULL);
}
