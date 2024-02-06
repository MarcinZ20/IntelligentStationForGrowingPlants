#ifndef INTELLIGENTSTATIONFORGROWINGPLANTS_MQTT_H
#define INTELLIGENTSTATIONFORGROWINGPLANTS_MQTT_H

// STANDARD LIBRARY
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// ESP INCLUDES
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include "mqtt_client.h"

// MY DEFINES
#define MQTT_URI        "mqtt://192.168.1.3:1883"
#define MQTT_PORT       1883
#define MQTT_USERNAME   "iot"
#define MQTT_PASSWORD   "iot"

#define MQTT_CONNECTED_BIT      BI0T
#define MQTT_FAIL_BIT           BIT1
#define MQTT_DISCONNECTED_BIT   BIT2
#define MQTT_PUBLISHED_BIT      BIT3

static EventGroupHandle_t s_mqtt_event_group;
static esp_mqtt_client_handle_t client = NULL;
static const char *MQTT_TAG = "[MQTT]";
static bool client_stopped = true;

static void log_error_if_nonzero(const char *message, int error_code);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void start_mqtt(void);
static void stop_mqtt(void);
static void mqtt_publish(const char* topic, const char* message);
void mqtt_publish_task(void *pvParameters);

#endif //INTELLIGENTSTATIONFORGROWINGPLANTS_MQTT_H
