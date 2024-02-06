#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_DISPLAY_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_DISPLAY_H

#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "ssd1306.c"


static ssd1306_handle_t ssd1306_dev = NULL;

static void config_display(void);
void display_task(void *pvParameter);
void display_string(const char *text, unsigned int time_s);
void display_clear(void);
// void display_info(const size_t option, const struct measurement measurement);

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_DISPLAY_H
