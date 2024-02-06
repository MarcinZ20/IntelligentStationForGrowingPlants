#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_UTILS_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_UTILS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "esp_wifi.h"
#include "esp_log.h"

struct measurement {
    float temperature;
    float humidity;
    float light;
} measurement;

static struct measurement current_measurement;

static struct measurement get_measurement();
static const char* get_mac_address();
char* create_topic(char* mac, char* user_id, char* topic_name);
static const char *convert_measurement_to_json_string(struct measurement measurement);

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_UTILS_H
