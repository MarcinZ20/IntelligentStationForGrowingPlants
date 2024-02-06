#include "include/pt550.h"

/*Description:
* Function to read the ADC value
*
* @Input: [void]
*
* @Output: [uint32_t] ADC value
*/
uint32_t read_adc(void) {
	return adc1_get_raw(ADC1_CHANNEL);
}

/*Description:
* Function to convert ADC value to voltage
*
* @Input:
*  - [uint32_t] adc_value: ADC value
*
* @Output: [float] Voltage
*/
float convert_adc_to_voltage(uint32_t adc_value) {
	return (adc_value * 3.3) / 4095;
}

/*Description:
* Function to convert ADC value to light intensity
*
* @Input:
*  - [uint32_t] adc_value: ADC value
*
* @Output: [float] Light intensity
*/
float convert_adc_to_light_intensity(uint32_t adc_value) {
	return (adc_value / 10.2);
}

/*Description:
* Function to read the light intensity
*
* @Input: [void]
*
* @Output: [float] Light intensity
*/
float read_light_intensity() {
    uint32_t adc_value = read_adc();
    return convert_adc_to_light_intensity(adc_value);
}

/*Description:
* Task to read the ADC value and print it
*
* @Input: [void*]
*
* @Output: [void] Prints ADC value
*/
void adc_task(void *pvParameter) {
	for (;;) {
		uint32_t adc_value = read_adc();
		float voltage = convert_adc_to_voltage(adc_value);
		float light_intensity = convert_adc_to_light_intensity(adc_value);
		printf("ADC Value: %ld\n", adc_value);
		printf("Voltage: %f\n", voltage);
		printf("Light Intensity: %f\n", light_intensity);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}