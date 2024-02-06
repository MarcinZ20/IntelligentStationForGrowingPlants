#include "include/switch.h"


/*Description:
 * Function to configure switch
 *
 * @Input: [void]
 *
 * @Output: [void] configures the switch on SWITCH_PIN_MAIN & SWITCH_PIN_RESET
 * */
static void config_switches(void) {

    gpio_config_t switch_config = {
            .pin_bit_mask = (1ULL << SWITCH_PIN_MAIN) | (1ULL << SWITCH_PIN_RESET),
            .mode = GPIO_MODE_DEF_INPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    gpio_config(&switch_config);

    s_switch_event_group = xEventGroupCreate();

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

    while (1) {
        switch_bits = xEventGroupWaitBits(s_switch_event_group, BIT_SWITCH_RESET, pdTRUE, pdTRUE, portMAX_DELAY);

        if (switch_bits & BIT_SWITCH_RESET) {
            ESP_LOGI(SWITCH_TAG, "Switch reset pressed");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void check_pressed(void* pvParameters) {
    printf("Check pressed task started\n");
    for (;;) {
        if (gpio_get_level(SWITCH_PIN_RESET) == 0) {
            ESP_LOGE(SWITCH_TAG, "Switch reset pressed");
            xEventGroupSetBits(s_switch_event_group, BIT_SWITCH_RESET);
            reset_flag = true;
        } else if (gpio_get_level(SWITCH_PIN_RESET) == 1) {
            ESP_LOGE(SWITCH_TAG, "Switch reset released");
            xEventGroupSetBits(s_switch_event_group, BIT_SWITCH_RESET_OFF);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}