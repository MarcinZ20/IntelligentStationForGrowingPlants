#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_I2C_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_I2C_H

#include "driver/i2c_master.h"
#include "stdio.h"
#include "stdlib.h"
#include "esp_log.h"

static const char *I2C_TAG = "[I2C]";

#define I2C_MASTER_SCL_IO 22        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */

static void config_i2c(void);
#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_I2C_H
