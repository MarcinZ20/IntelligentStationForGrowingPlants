#include "nvs_flash.h"
#include "esp_log.h"

#define WIFI_SSID_KEY    "wifi_ssid"
#define WIFI_PASS_KEY    "wifi_password"

static const char *NVS_TAG = "[NVS]";

void save_wifi_credentials(const char *ssid, const char *password) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Open NVS namespace
    nvs_handle_t nvs_handle;
    ret = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle", esp_err_to_name(ret));
        return;
    }

    // Write SSID to NVS
    ret = nvs_set_str(nvs_handle, WIFI_SSID_KEY, ssid);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) writing SSID to NVS", esp_err_to_name(ret));
    }

    // Write password to NVS
    ret = nvs_set_str(nvs_handle, WIFI_PASS_KEY, password);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) writing password to NVS", esp_err_to_name(ret));
    }

    // Commit changes
    ret = nvs_commit(nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) committing changes to NVS", esp_err_to_name(ret));
    }

    // Close NVS
    nvs_close(nvs_handle);
}

void read_wifi_credentials(char *ssid, char *password, size_t max_len) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Open NVS namespace
    nvs_handle_t nvs_handle;
    ret = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle", esp_err_to_name(ret));
        return;
    }

    // Read SSID from NVS
    size_t ssid_len = max_len;
    ret = nvs_get_str(nvs_handle, WIFI_SSID_KEY, ssid, &ssid_len);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) reading SSID from NVS", esp_err_to_name(ret));
    }

    // Read password from NVS
    size_t pass_len = max_len;
    ret = nvs_get_str(nvs_handle, WIFI_PASS_KEY, password, &pass_len);
    if (ret != ESP_OK) {
        ESP_LOGE(NVS_TAG, "Error (%s) reading password from NVS", esp_err_to_name(ret));
    }

    // Close NVS
    nvs_close(nvs_handle);
}