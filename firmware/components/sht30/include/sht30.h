#ifndef SHT30_H
#define SHT30_H

#include "driver/i2c.h"
#include "esp_err.h"

#define SHT30_ADD 0x44

esp_err_t sht30_read(i2c_port_t i2c_num, float *temp, float *hum);

#endif
