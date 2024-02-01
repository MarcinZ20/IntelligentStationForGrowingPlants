#include "include/i2c.h"

/*Description:
 * Function to configure and initialize I2C protocol on the esp32 board
 *
 * @Input: [void]
 *
 * @Output: [void] I2C module is initialized and ready to work with
 * */
static void config_i2c(void) {

	i2c_config_t i2c_config = {
			.mode = I2C_MODE_MASTER,
			.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
			.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
			.sda_pullup_en = GPIO_PULLUP_ENABLE,
			.scl_pullup_en = GPIO_PULLUP_ENABLE,
			.master.clk_speed = I2C_MASTER_FREQ_HZ,
			.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
	};

	i2c_param_config(I2C_MASTER_NUM, &i2c_config);
	i2c_driver_install(I2C_MASTER_NUM, i2c_config.mode, 0, 0, 0);
	ESP_LOGI(I2C_TAG, "Module configuration successful!");
}
