#include "include/adc.h"

static void config_adc(void) {
	adc1_config_channel_atten(ADC1_CHANNEL, ADC_ATTENUATION);

	esp_adc_cal_characteristics_t adc1_chars;
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
	adc1_config_width(ADC_WIDTH_BIT_DEFAULT);
}