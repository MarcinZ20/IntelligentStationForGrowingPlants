#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_ADC_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_ADC_H

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_adc/adc_oneshot.h"

#include "stdio.h"
#include "stdlib.h"

#define ADC1_CHANNEL ADC1_CHANNEL_6
#define ADC_ATTENUATION ADC_ATTEN_DB_11

static const char *PT550_TAG = "PT-550";
volatile static esp_adc_cal_characteristics_t adc1_chars;

static void config_adc(void);
#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_ADC_H
