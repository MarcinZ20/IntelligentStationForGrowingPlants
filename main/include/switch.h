#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_SWITCH_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_SWITCH_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "stdio.h"

#define BIT_SWITCH_MAIN BIT0
#define BIT_SWITCH_RESET BIT1

#define SWITCH_PIN_MAIN GPIO_NUM_25
#define SWITCH_PIN_RESET GPIO_NUM_19

static const char* SWITCH_TAG = "[SWITCH]";
static EventGroupHandle_t s_switch_event_group;

static void config_switch(void);
void click_switch_task(void *pvParameters);
void reset_switch_task(void *pvParameters);

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_SWITCH_H
