#define LED_GPIO_PIN 2

void blinkLED(void) {
    for (int i = 0; i < 5; i++) {
        gpio_set_level(LED_GPIO_PIN, 1);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(LED_GPIO_PIN, 0);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}