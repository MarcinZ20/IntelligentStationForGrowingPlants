#include "include/led.h"

/* Description:
 * This function configures green & red LED lights defined in the project
 *
 * @Input: [void]
 *
 * @Output: none, but each led will be configured
 */
static void config_led(void) {
    gpio_config_t green_led_config = {
            .pin_bit_mask = (1ULL << LED_GREEN_PIN),
            .mode = GPIO_MODE_OUTPUT
    };

    gpio_config_t red_led_config = {
            .pin_bit_mask = (1ULL << LED_RED_PIN),
            .mode = GPIO_MODE_OUTPUT
    };

    gpio_config(&green_led_config);
    gpio_config(&red_led_config);
    s_led_event_group = xEventGroupCreate();
    ESP_LOGI(LED_TAG, "LEDs configured!");
}

/*Description:
 *Function that performs blinking with green led
 *
 * @Input: [void*] pvParameters: gpio pin of the chosen LED
 *
 * @Output: [void] Blinks chosen led
 * */
void blink_led_task(void *pvParameters) {
    int8_t gpio_pin = (int8_t)pvParameters;
	printf("Gpio pin: %d\n", gpio_pin);

    for (;;) {
        gpio_set_level(gpio_pin, 1);
        vTaskDelay(LED_BLINK_RATE_MS / portTICK_PERIOD_MS);
        gpio_set_level(gpio_pin, 0);
        vTaskDelay(LED_BLINK_RATE_MS / portTICK_PERIOD_MS);
    }
}

/*Description:
 * Function that turns on the desired LED
 *
 * @Input: [void*] pvParameters -> gpio pin of the chosen LED
 *
 * @Output: [void] Lights the chosen LED
 * */
void beam_led_task(void *pvParameters) {
    int8_t gpio_pin = (int8_t*)pvParameters;
	printf("Gpio pin: %d\n", gpio_pin);

    for (;;) {
        gpio_set_level(gpio_pin, 1);
        vTaskDelay(1);
    }
}

/*Description:
 * Function that checks for a signal to turn start/stop blinking the led
 *
 * @Input: [void*] pvParameters -> gpio pin of the chosen LED
 *
 * @Output: [void] Turns off the chosen LED
 * */
void check_led_task(void *pvParameters) {

    if ((int)pvParameters == NULL) {
        ESP_LOGE(LED_TAG, "No LED pin provided!\n");
    }

    EventBits_t led_bits;
    TaskHandle_t handle_led_blink = NULL;
    xTaskCreate(&blink_led_task, "blink_led", 2000, (void*)LED_GREEN_PIN, 10, &handle_led_blink);

    for (;;) {
        led_bits = xEventGroupWaitBits(s_led_event_group, BIT_LED_BLINK | BIT_LED_NOT_BLINK, pdTRUE, pdFALSE, portMAX_DELAY);
        if (led_bits & BIT_LED_BLINK) {
            vTaskResume(handle_led_blink);
        } else if (led_bits & BIT_LED_NOT_BLINK) {
            vTaskSuspend(handle_led_blink);
            gpio_set_level(LED_GREEN_PIN, 0);
        }
        vTaskDelay(1);
    }
}
