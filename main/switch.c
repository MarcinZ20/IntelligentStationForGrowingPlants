#include "include/switch.h"

/*Description:
 * ISR handler for switch
 *
 * @Input:
 *  - [void*] arg: argument passed to the handler
 *
 * @Output: [void] sets the event group bit
 * */
static void IRAM_ATTR switch_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio_num == SWITCH_PIN_MAIN) {
        ESP_LOGE(SWITCH_TAG, "Main switch pressed!");
        xEventGroupSetBitsFromISR(s_switch_event_group, BIT_SWITCH_MAIN, &xHigherPriorityTaskWoken);
    } else if (gpio_num == SWITCH_PIN_RESET) {
        ESP_LOGE(SWITCH_TAG, "Reset switch pressed!");
        xEventGroupSetBitsFromISR(s_switch_event_group, BIT_SWITCH_RESET, &xHigherPriorityTaskWoken);
    }

    if (xHigherPriorityTaskWoken) {
        printf("Higher priority task woken\n");
        portYIELD_FROM_ISR();
    }
}

/*Description:
 * Function to configure switch
 *
 * @Input: [void]
 *
 * @Output: [void] configures the switch on SWITCH_PIN_MAIN & SWITCH_PIN_RESET
 * */
static void config_switch(void) {

    gpio_config_t switch_config = {
            .pin_bit_mask = (1ULL << SWITCH_PIN_MAIN) | (1ULL << SWITCH_PIN_RESET),
            .mode = GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_ANYEDGE,
            .pull_up_en = 1,
    };

    gpio_config(&switch_config);

    s_switch_event_group = xEventGroupCreate();

    gpio_install_isr_service(0);

    gpio_isr_handler_add(SWITCH_PIN_MAIN, switch_isr_handler, NULL);
    gpio_isr_handler_add(SWITCH_PIN_RESET, switch_isr_handler, NULL);

	ESP_LOGI(SWITCH_TAG, "Configuration successful!");
}

/*Description:
 * Function to check if the switch is pressed
 *
 * @Input: [void]
 *
 * @Output: [bool] returns true if the switch is pressed
 * */
void switch_task(void *pvParameters) {
    EventBits_t switch_bits;

    for (;;) {
        switch_bits = xEventGroupWaitBits(s_switch_event_group, BIT_SWITCH_MAIN | BIT_SWITCH_RESET, pdTRUE, pdFALSE,
                                          portMAX_DELAY);

        if (switch_bits & BIT_SWITCH_MAIN) {
            ESP_LOGI(SWITCH_TAG, "Main switch pressed!");
            // xEventGroupSetBits(s_led_event_group, BIT_LED_BLINK);
            // xEventGroupSetBits(s_mqtt_event_group, MQTT_PUBLISH_BIT);
        } else if (switch_bits & BIT_SWITCH_RESET) {
            ESP_LOGI(SWITCH_TAG, "Reset switch pressed!");
            // xEventGroupSetBits(s_led_event_group, BIT_LED_BLINK);
            // xEventGroupSetBits(s_mqtt_event_group, MQTT_RESET_BIT);
        }
    }
}