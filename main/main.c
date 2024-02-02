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
//#include "adc.c"


void app_main (void) {

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

	EventBits_t wifi_bits;
	EventBits_t mqtt_bits;
	EventBits_t led_bits;
	EventBits_t switch_bits;

//  COMPONENT CONFIGURATION
	config_i2c();
	config_switch();
	config_display();
	config_led();
//	config_adc();
	config_wifi();

//  Event Handlers
	TaskHandle_t handle_display = NULL;
	TaskHandle_t handle_check_led = NULL;
	TaskHandle_t handle_mqtt_publish = NULL;
	TaskHandle_t handle_mqtt = NULL;
	TaskHandle_t handle_reset = NULL;
	TaskHandle_t handle_greet_message = NULL;

	//  TASKS
	xTaskCreate(&check_led_task, "check_led", 2000, NULL, 2, &handle_check_led);
	xTaskCreate(&greet_user_task, "greet_user", 2000, NULL, 10, &handle_greet_message);

	sleep(3); // To greet user

	//  MAIN LOOP
	for (;;) {

		// switch_bits = xEventGroupWaitBits(s_switch_event_group, BIT_SWITCH_MAIN | BIT_SWITCH_RESET, pdTRUE, pdFALSE, portMAX_DELAY);
		wifi_bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT | WIFI_DISCONNECTED_BIT,
		                                pdTRUE, pdFALSE, portMAX_DELAY);

		if (wifi_bits & WIFI_CONNECTED_BIT) {
			display_string("Wi-Fi connected", 3);
			xEventGroupSetBits(s_led_event_group, BIT_LED_NOT_BLINK);
			start_mqtt();
			mqtt_bits = xEventGroupWaitBits(s_mqtt_event_group, MQTT_CONNECTED_BIT | MQTT_FAIL_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
			if (mqtt_bits & MQTT_CONNECTED_BIT) {
				xTaskCreate(&mqtt_publish_task, "mqtt_publish_task", 20000, (void *) 5, 3, &handle_mqtt_publish);
			} else if (mqtt_bits & MQTT_FAIL_BIT) {
				;
			} else {
				;
			}
		} else if (wifi_bits & WIFI_DISCONNECTED_BIT) {
			if (!client_stopped) {
				vTaskDelete(handle_mqtt_publish);
				stop_mqtt();
			}
			xEventGroupSetBits(s_led_event_group, BIT_LED_BLINK);
			display_string("Wi-Fi down ...", 0);
		} else if (wifi_bits & WIFI_FAIL_BIT) {
			ESP_LOGE("[APP]", "COULDN'T CONNECT TO WIFI");
		}
	}
}