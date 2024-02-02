#include "include/switch.h"

/*Description:
 * Function to configure switch
 *
 * @Input: [void]
 *
 * @Output: [void] configures the switch on SWITCH_PIN_MAIN & SWITCH_PIN_RESET
 * */
static void config_switch(void) {

    gpio_config_t main_switch_config = {
            .pin_bit_mask = (1ULL << SWITCH_PIN_MAIN),
            .mode = GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_ANYEDGE
    };

	gpio_config_t reset_switch_config = {
			.pin_bit_mask = (1ULL << SWITCH_PIN_RESET),
			.mode = GPIO_MODE_INPUT,
			.intr_type = GPIO_INTR_ANYEDGE
	};

    gpio_config(&main_switch_config);
	gpio_config(&reset_switch_config);
    s_switch_event_group = xEventGroupCreate();
	ESP_LOGI(SWITCH_TAG, "Configuration successful!");
}

/*Description:
 * Task function that checks if switch is being clicked
 *
 * @Input: [void*] pvParameters: none
 *
 * @Output: [void] performs an action when clicked
 * */
void click_switch_task(void *pvParameters) {
    for (;;) {
        if (gpio_get_level(SWITCH_PIN_MAIN)) {
            gpio_set_level(33, 1);
	        printf("Main pin works");
        } else {
            gpio_set_level(33, 0);
        }
        vTaskDelay(1);
    }
}

/*Description:
 * Task function that checks if switch is being clicked for 3 seconds
 *
 * @Input: [void*] pvParameters: none
 *
 * @Output: [void] performs an action when clicked for 3 seconds
 * */
void reset_switch_task(void *pvParameters) {
	printf("Reset task running!\n");

	if (gpio_get_level(SWITCH_PIN_RESET)) {
		printf("Reset pin works!");
        xEventGroupSetBits(s_switch_event_group, BIT_SWITCH_RESET);
	}
}