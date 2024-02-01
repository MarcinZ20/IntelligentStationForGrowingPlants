#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_LED_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_LED_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "stdio.h"

#define BIT_LED_OFF BIT0
#define BIT_LED_ON  BIT1
#define BIT_LED_BLINK BIT2
#define BIT_LED_NOT_BLINK BIT3

#define LED_BLINK_RATE_MS 150

static int LED_RED_PIN = 32;
static int LED_GREEN_PIN = 33;

static EventGroupHandle_t s_led_event_group;
static const char* LED_TAG = "[LED]";

static void config_led(void);
void blink_led_task(void *pvParameters);
void beam_led_task(void *pvParameters);
void check_led_task(void *pvParameters);

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_LED_H
