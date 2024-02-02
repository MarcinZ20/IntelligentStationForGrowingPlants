#include "include/pt550.h"

//void pt550_task(char *pvParameters) {
//
//	for (;;) {
//		int raw_value = adc1_get_raw(ADC1_CHANNEL);
//		uint32_t mV = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL), &adc1_chars);
//		printf("Odczyt adc: %ld\n", mV);
//		vTaskDelay(1000);
//	}
//}

//float read_light_intensity() {
//	int raw_value = adc1_get_raw(ADC_CHANNEL);
////	uint32_t mv = esp_adc_calc_raw_to_voltage(raw_value, );
//	return (float)raw_value;
//}