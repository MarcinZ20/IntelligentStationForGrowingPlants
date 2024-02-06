#include "include/display.h"

/*Description:
 * Function to initialize the OLED display (using I2C protocol)
 *
 * @Input: [void]
 *
 * @Output: [void] Initialized display with set properties
 * */
void config_display(void) {

    ssd1306_dev = ssd1306_create(I2C_NUM_1, SSD1306_I2C_ADDRESS);
}

/*Description:
 * Task that displays text on the OLED display using #display_string function
 *
 * @Input: [void*] char* cast to void* with the desired text
 *
 * @Output: [void] Text is displayed on the screen
 * */
void display_task(void *pvParameter) {
    const char *message = (char*)pvParameter;

    for (;;) {
        display_string(message, 0);
        vTaskDelete(NULL);
    }
}

/*Description:
 * Function to display text on the OLED screen
 *
 * @Input:
 *  - [const char*] text: text to display
 *  - [unsigned int] time_s: time[s] after which the display will clear. Set time_s to 0, to avoid clearing the screen.
 *
 * @Output: [void] Display will show the desired text
 * */
void display_string(const char *text, unsigned int time_s) {
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    char text_buffer[20] = {0};
    sprintf(text_buffer, "%s", text);
    printf("%s\n", text_buffer);

    ssd1306_draw_string(ssd1306_dev, 1, 16, (const uint8_t *)text_buffer, 16, 1);
    ssd1306_refresh_gram(ssd1306_dev);

    if (time_s != 0) {
        sleep(time_s);
        ssd1306_clear_screen(ssd1306_dev, 0x00);
        ssd1306_refresh_gram(ssd1306_dev);
        return;
    }
}
