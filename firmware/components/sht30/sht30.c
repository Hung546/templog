#include "sht30.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

esp_err_t sht30_read(i2c_port_t i2c_num, float *temp, float *hum) {
  esp_err_t err;
  uint8_t cmd[2] = {0x24, 0x00};
  uint8_t data[6];

  i2c_cmd_handle_t cmd_handler = i2c_cmd_link_create();
  i2c_master_start(cmd_handler);
  i2c_master_write_byte(cmd_handler, (SHT30_ADD << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write(cmd_handler, cmd, sizeof(cmd), true);
  i2c_master_stop(cmd_handler);
  err = i2c_master_cmd_begin(i2c_num, cmd_handler, pdMS_TO_TICKS(1000));
  i2c_cmd_link_delete(cmd_handler);

  if (err != ESP_OK)
    return err;

  vTaskDelay(pdMS_TO_TICKS(20));

  cmd_handler = i2c_cmd_link_create();
  i2c_master_start(cmd_handler);
  i2c_master_write_byte(cmd_handler, (SHT30_ADD << 1) | I2C_MASTER_READ, true);
  i2c_master_read(cmd_handler, data, 5, I2C_MASTER_ACK);
  i2c_master_read_byte(cmd_handler, data + 5, I2C_MASTER_NACK);
  i2c_master_stop(cmd_handler);

  err = i2c_master_cmd_begin(i2c_num, cmd_handler, pdMS_TO_TICKS(1000));
  i2c_cmd_link_delete(cmd_handler);

  if (err == ESP_OK) {
    uint16_t raw_temp = (data[0] << 8) | data[1];
    uint16_t raw_hum = (data[3] << 8) | data[4];

    *temp = -45 + 175 * (raw_temp / (65535.0));
    *hum = 100 * (raw_hum / (65535.0));
  }
  return err;
}
