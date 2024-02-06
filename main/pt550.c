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
    float measurement = pt550_measure_with_mean();
	return measurement;
}

/*Description:
* Function to measure the light intensity with mean
*
* @Input: [void]
*
* @Output: [void] Returns the mean of 5 light intensity measurements
*/
float pt550_measure_with_mean(void) {
	float light_measurements_sum = 0;

	for (size_t i = 0; i < 5; i++) {
		uint32_t adc_value = read_adc();
		float light_intensity = convert_adc_to_light_intensity(adc_value);
		light_measurements_sum += light_intensity;
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	float light_intensity_mean = light_measurements_sum / 5;
	return light_intensity_mean;
}