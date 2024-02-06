#include "include/am_2320.h"
 
//static esp_err_t i2c_master_am2320_read(uint8_t *data, size_t size) {
//	if (size < 4) {
//		return ESP_ERR_INVALID_SIZE;
//	}
//
//	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//
//	i2c_master_start(cmd);
//	i2c_master_write_byte(cmd, (AM2320_ADDRESS << 1) | I2C_MASTER_WRITE, true);
//	i2c_master_write_byte(cmd, 0x03, true);
//	i2c_master_stop(cmd);
//	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
//	i2c_cmd_link_delete(cmd);
//
//	vTaskDelay(2 / portTICK_PERIOD_MS);
//
//	cmd = i2c_cmd_link_create();
//
//	i2c_master_start(cmd);
//	i2c_master_write_byte(cmd, (AM2320_ADDRESS << 1) | I2C_MASTER_READ, true);
//	i2c_master_read_byte(cmd, &data[0], I2C_MASTER_ACK);
//	i2c_master_read_byte(cmd, &data[1], I2C_MASTER_ACK);
//	i2c_master_read_byte(cmd, &data[2], I2C_MASTER_ACK);
//	i2c_master_read_byte(cmd, &data[3], I2C_MASTER_NACK);
//	i2c_master_stop(cmd);
//	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
//	i2c_cmd_link_delete(cmd);
//
//	return ESP_OK;
//}

static esp_err_t read_am2320_sensor(uint8_t *data, size_t size) {
	if (size != 8) {
		ESP_LOGE("[AM-2320]", "Wrong input length");
	}

	// i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	// i2c_master_start(cmd);

	// // Write the sensor address with the write bit
	// i2c_master_write_byte(cmd, (AM2320_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	// i2c_master_write_byte(cmd, AM2320_CMD_READ, true);

	// i2c_master_write_byte(cmd, AM2320_REG_HUMIDITY, true);
	// i2c_master_write_byte(cmd, AM2320_REG_TEMPERATURE, true);

	// // Write the sensor address with the read bit
	// i2c_master_write_byte(cmd, (AM2320_ADDRESS << 1) | I2C_MASTER_READ, false);

	// // Read the humidity and temperature data
	// i2c_master_read(cmd, data, 8, I2C_MASTER_LAST_NACK);
	// printf("Data: %d %d %d %d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

	// // Stop I2C communication
	// i2c_master_stop(cmd);

	// // Execute the command
	// esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_PERIOD_MS);

	// if (ret == ESP_OK) {
	// 	uint16_t humidity = (data[2] << 8) | data[3];
	// 	uint16_t temperature = (data[4] << 8) | data[5];
	// 	printf("Humidity: %.1f%%\n", humidity / 10.0);
	// 	printf("Temperature: %.1f°C\n", temperature / 10.0);
	// } else {
	// 	ESP_LOGE("AM2320", "Failed to read data: %s", esp_err_to_name(ret));
	// }

	// i2c_cmd_link_delete(cmd);
	// esp_err_t ret = i2c_master_write_read_device(I2C_MASTER_NUM, AM2320_ADDRESS, AM2320_CMD_READ, 1, data, 8, 1000 / portTICK_PERIOD_MS);
	float temperature;
	float humidity;
	i2cdev_init();

	i2c_config_t i2c_config_am2320 = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = (gpio_num_t)21,
		.scl_io_num = (gpio_num_t)22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 100000,
		.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
	};


	i2c_dev_t dev = {
		.port = I2C_NUM_1,
		.cfg = i2c_config_am2320,
		.addr = AM2320_ADDRESS,
		.mutex = NULL,
		.timeout_ticks = 0
	};
	

	am2320_init_desc(&dev, I2C_MASTER_NUM , (gpio_num_t)21, (gpio_num_t)22);
	esp_err_t ret = am2320_get_rht(&dev, &temperature, &humidity);
	printf("Temperature: %.1f°C\n", temperature);
	printf("Humidity: %.1f%%\n", humidity);

	if (ret == ESP_OK) {
		// printf("Data: %d %d %d %d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	} else {
		ESP_LOGE("AM2320", "Failed to read data: %s", esp_err_to_name(ret));
	}
	i2cdev_done();
	return ESP_OK;
}



void am2320_task(void *pvParameters) {

	uint8_t am2320_data[4];

	for (;;) {
		if (read_am2320_sensor(am2320_data, sizeof(am2320_data)) == ESP_OK) {
			vTaskDelay(5000 / portTICK_PERIOD_MS);
		}
	}
}
