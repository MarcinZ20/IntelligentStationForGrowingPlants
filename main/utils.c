#include "include/utils.h"
#include "am_2320.c"
#include "pt550.c"

/*Description:
 * Function to get measurements from AM-2320 & PT-500 sensors
 *
 * @Input: [void]
 *
 * @Output: [void] Writes the data into 'static const struct measurement'
 * */
static struct measurement get_measurement () {
	struct measurement measurement;

	uint8_t am2320_data[8];

	if (read_am2320_sensor(am2320_data, sizeof(am2320_data)) == ESP_OK) {
		measurement.humidity = ((am2320_data[2] << 8) | am2320_data[3]) * 0.1;
		measurement.temperature = ((am2320_data[0] << 8) | am2320_data[1]) * 0.1;
	} else {
		printf("Błąd odczytu danych z czujnika AM2320\n");
	}

	measurement.light = read_light_intensity();

	current_measurement = measurement;

	return measurement;
}

/*Description:
 * Function to convert 'static const struct measurement' to JSON string
 *
 * @Input: [void]
 *
 * @Output: [const char*] result: JSON string with measurement results
 * */
static const char *convert_measurement_to_json_string(struct measurement measurement) {
	char *result = (char *) malloc(200);

	sprintf(result,
	        "{\n    \"data\": {\n        \"temperature\": %.2f,\n        \"humidity\": %.2f,\n        \"light\": %.2f\n    }\n}",
	        measurement.temperature, measurement.humidity, measurement.light);

	return result;
}

/*Description:
 * Function to get unique MAC address of the esp32 unit
 *
 * @Input: [void]
 *
 * @Output: [char*] Pointer to the array containing mac address numbers
 * */
const char* get_mac_address() {
	uint8_t mac[6];
	esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);

	char* mac_str = malloc(sizeof(char*) * 18); // Allocate memory for MAC string "xx:xx:xx:xx:xx:xx\0"

	if (!mac_str) {
		printf("Memory allocation failed\n");
		return NULL;
	}

	snprintf(mac_str, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	printf("MAC address of ESP32: %s\n", mac_str);

	return mac_str;
}


