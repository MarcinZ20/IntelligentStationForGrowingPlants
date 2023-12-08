// ESP project files
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

// MY FILES
#include "wifi.c"
#include "mqtt.c"
#include "ble-server.c"
//#include "ble-client.c"

void app_main(void) {
  esp_rom_gpio_pad_select_gpio(LED_GPIO_PIN);
  gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_init_sta();
  mqtt_app_start();
  ble_server_run();
//  ble_client_run();
}
