// ESP project files
#include "esp_system.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

// MY FILES
#include "led.c"
#include "switch.c"
#include "display.c"
#include "wifi.c"
#include "mqtt.c"
#include "i2c.c"
#include "adc.c"
#include "ble-server.c"
#include "nvs_save.c"
#include "driver/gpio.h"

void send_put() {
	char res[200];
	char *mac_addr = get_mac_address();
	printf("MAC: %s\n", mac_addr);
	snprintf(res, sizeof(res), "{\"devicesIds\":[\"%s\"]}", mac_addr);
	http_put("192.168.43.81", "/api/devices", 8000, res, user_token);
}

void reset(EventBits_t wifi_bits, EventBits_t ble_bits, TaskHandle_t handle_mqtt_publish) {
	ESP_LOGI("[APP]", "RESETING DEVICE");
	display_string("Resetting ...", 0);
	reset_flag = false;

	if (wifi_connected) {
		vTaskDelete(handle_mqtt_publish);
		ESP_LOGI("RESET", "AFTER MQTT DELETE");
		stop_mqtt();
		ESP_LOGI("RESET", "AFTER MQTT STOP");
		esp_wifi_disconnect();
	}

	ESP_LOGI("RESET", "BEFORE BLE BITS");
	ble_bits = xEventGroupWaitBits(s_ble_event_group, BLE_CONNECTED_BIT, pdTRUE, pdFALSE, 60000 / portTICK_PERIOD_MS);
	ESP_LOGI("RESET", "AFTER 1 BLE BITS");
	if (ble_bits & BLE_CONNECTED_BIT) {
		ESP_LOGI("RESET", "SSID: %s\n", user_ssid);
		ESP_LOGI("RESET", "Password: %s\n", user_password);
		save_wifi_credentials(user_ssid, user_password);
	} else {
		return;
	}

	ESP_LOGI("RESET", "AFTER BLE BITS");
	set_wifi_config(user_ssid, user_password);
	start_wifi();

	wifi_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdTRUE, pdFALSE, 10000 / portTICK_PERIOD_MS);
	if (wifi_bits & WIFI_CONNECTED_BIT) {
		send_put();
	} else {
		return;
	}
	reset_flag = false;
}

void app_main (void) {

	ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

//  Initialize NVS
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
	    ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);

	EventBits_t wifi_bits = NULL;
	EventBits_t mqtt_bits = NULL;
	EventBits_t led_bits = NULL;
	EventBits_t switch_bits = NULL;
	EventBits_t ble_bits = NULL;

//  COMPONENT CONFIGURATION
	config_i2c();
	config_switches();
	config_display();
	config_led();
	config_adc();
	ble_server_run();
	config_wifi();

// Display startup message
	display_string("Booting ...", 2);
	display_string("Hello there!", 3);

//  Event Handlers
	TaskHandle_t handle_check_led = NULL;
	TaskHandle_t handle_mqtt_publish = NULL;
	TaskHandle_t handle_reset = NULL;

	//  TASKS
	xTaskCreate(&check_led_task, "check_led", 2000, (void*)LED_GREEN_PIN, 2, &handle_check_led);
	xTaskCreatePinnedToCore(&check_pressed, "check_pressed", 2000, NULL, 10, &handle_reset, 1);

//  Read Wi-Fi credentials from NVS
	read_wifi_credentials(user_ssid, user_password, 50);

	if (strlen(user_ssid) == 0 || strlen(user_password) == 0) {
		ESP_LOGE("[APP]", "COULDN'T READ WIFI CREDENTIALS");

		switch_bits = xEventGroupWaitBits(s_switch_event_group, BIT_SWITCH_RESET, pdTRUE, pdFALSE, portMAX_DELAY);	

		if (switch_bits & BIT_SWITCH_RESET) {
			reset(wifi_bits, ble_bits, handle_mqtt_publish);
		} 
	} else {
		set_wifi_config(user_ssid, user_password);
		start_wifi();
	}

	printf("SSID: %s\n", user_ssid);
	printf("Password: %s\n", user_password);

	//  MAIN LOOP
	for (;;) {
		ESP_LOGE("[APP]", "LOOPING");

		// wifi_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT | WIFI_DISCONNECTED_BIT,
		//                                 pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
		switch_bits = xEventGroupWaitBits(s_switch_event_group, BIT_SWITCH_RESET_OFF | BIT_SWITCH_RESET, pdTRUE, pdFALSE, portMAX_DELAY);
		
		if (reset_flag) {
			reset(wifi_bits, ble_bits, handle_mqtt_publish);
		} else if (wifi_connected && (switch_bits & BIT_SWITCH_RESET_OFF) && client_stopped) {
			xEventGroupSetBits(s_led_event_group, BIT_LED_NOT_BLINK);
			display_string("Wi-Fi connected", 3);

			start_mqtt();

			mqtt_bits = xEventGroupWaitBits(s_mqtt_event_group, MQTT_CONNECTED_BIT | MQTT_FAIL_BIT | MQTT_PUBLISHED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

			ESP_LOGE("[APP]", "MQTT BITS");

			if (mqtt_bits & MQTT_CONNECTED_BIT) {
				xTaskCreate(&mqtt_publish_task, "mqtt_publish_task", 20000, (void *) 5, 3, &handle_mqtt_publish);
			} else if (mqtt_bits & MQTT_FAIL_BIT) {
				ESP_LOGE("[APP]", "COULDN'T CONNECT TO MQTT");
			} else {
				ESP_LOGE("[APP]", "UNKNOWN MQTT ERROR");
			}

		} else if (!wifi_connected && (switch_bits & BIT_SWITCH_RESET_OFF)) {
			if (!client_stopped) {
				vTaskDelete(handle_mqtt_publish);
				stop_mqtt();
			}
			xEventGroupSetBits(s_led_event_group, BIT_LED_BLINK);
			display_string("Wi-Fi down ...", 0);
		}
	}
}