#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_PT550_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_PT550_H

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_adc/adc_oneshot.h"

#include "stdio.h"
#include "stdlib.h"
#include "inttypes.h"

#define ADC1_CHANNEL ADC1_CHANNEL_4

uint32_t read_adc(void);
float convert_adc_to_voltage(uint32_t adc_value);
float convert_adc_to_light_intensity(uint32_t adc_value);
float read_light_intensity(void);
void adc_task(void *pvParameter);

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_PT550_H
