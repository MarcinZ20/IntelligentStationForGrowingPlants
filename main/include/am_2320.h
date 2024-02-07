#include "stdlib.h"
#include "stdio.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>



//static const char *AM_TAG = "AM-2320";

// #define I2C_MASTER_NUM I2C_NUM_1
// #define AM2320_ADDRESS 0xB8
// #define TIMEOUT_MS 1000
// #define DELAY_MS 1000
// #define AM2320_CMD_READ 0x03
// #define AM2320_REG_HUMIDITY 0x00
// #define AM2320_REG_TEMPERATURE 0x04

// void am2320_task(void *pvParameters);
// static esp_err_t read_am2320_sensor(uint8_t *data, size_t size);
// //static esp_err_t i2c_master_am2320_read(uint8_t *data, size_t size);
