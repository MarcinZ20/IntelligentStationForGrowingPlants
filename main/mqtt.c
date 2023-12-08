// STANDARD LIBRARY
#include <stddef.h>
#include <string.h>
#include <unistd.h>

// ESP INCLUDES
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include "mqtt_client.h"

// MY DEFINES
#define MQTT_URI        "mqtt://test.mosquitto.org"
#define MQTT_PORT       1883
#define MQTT_TOPIC      "/topic/qos0"
#define MQTT_USERNAME   "iot"
#define MQTT_PASSWORD   "iot"

static const char *MQTT_TAG = "MQTT";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(MQTT_TAG, "ESTABLISHING CONNECTION ...");
            break;
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "CONNECTION TO SERVER SUCCESSFUL");
            for (size_t i = 0; i < 5; i++) {
                msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC, "Hello from ESP-32!", 0, 1, 0);
                ESP_LOGI(MQTT_TAG, "Sent publish successful, msg_id: %d", msg_id);
                sleep(3);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_TAG, "EVENT_PUBLISHED, msg_id: %d", event->msg_id);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(MQTT_TAG, "EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(MQTT_TAG, "Other event id: %d", event->event_id);
            break;
    }
}

static void mqtt_app_start(void)
{
    struct authentication_t authentication = {
            .password = MQTT_PASSWORD
    };

    esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = MQTT_URI,
            .broker.address.port = MQTT_PORT,
//            .credentials.username = MQTT_USERNAME,
//            .credentials.authentication = authentication
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}