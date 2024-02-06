#include "include/mqtt.h"
#include "utils.c"

/*Description:
 * Error thrower
 *
 * @Input:
 *  - [const char*] message: error message
 *  - [int] error_code: self-explanatory
 *
 * */
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*Description:
 * Event handler for mqtt module
 *
 * @Input:
 *  - [void*] handler_args: specific args for handler
 *  - [esp_event_base_t] base: base event
 *  - [int32_t] event_id: id of the incomming event
 *  - [void*] event_data: data of the incoming event
 *
 *  @Output:
 *  - [void] Only handles mqtt events
 * */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {

//    ESP_LOGE(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(MQTT_TAG, "ESTABLISHING CONNECTION ...");
            break;
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "CONNECTION TO SERVER SUCCESSFUL");
			xEventGroupSetBits(s_mqtt_event_group, MQTT_CONNECTED_BIT);
			client_stopped = false;
			break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "EVENT_DISCONNECTED");
		    xEventGroupSetBits(s_mqtt_event_group, MQTT_DISCONNECTED_BIT);
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

/*Description:
 * Function to initialize mqtt module in the program
 *
 * @Input: [void]
 *
 * @Output: [void] Initializes mqtt module
 * */
static void start_mqtt(void) {
	s_mqtt_event_group = xEventGroupCreate();

    esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = MQTT_URI,
            .broker.address.port = MQTT_PORT,
    };

	client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/*Description:
 * Function to stop mqtt client
 *
 * @Input: [void]
 *
 * @Output: [void] Sets the client_stopped flag to true and stops the client
 * */
static void stop_mqtt(void) {
	ESP_LOGE(MQTT_TAG, "CLIENT STOPPED");
	esp_mqtt_client_stop(client);
	client_stopped = true;
}

/*Description:
 * Helper function used in mqtt_publish_task for better readability
 *
 * @Input:
 *  - [const char*] topic: topic
 *  - [const char*] message: data to publish
 *
 *  @Output: [void] Publishes message on topic
 *  */
void mqtt_publish(const char* topic, const char* message) {
	int msg_id = esp_mqtt_client_publish(client, topic, message, 0, 1, 0);

	switch (msg_id) {
		case -1:
			ESP_LOGE(MQTT_TAG, "Publish failed");
		case -2:
			ESP_LOGI(MQTT_TAG, "Something went wrong, msg_id: %d", msg_id);
		default:
			ESP_LOGI(MQTT_TAG, "Sent publish successful, msg_id: %d", msg_id);
	}
}

/*Description:
 * Task for publishing data with mqtt protocol
 *
 * @Input:
 *  - [void*] pvParameters: time gap in [s] between each publish
 *
 * @Output:
 *  - [void] none
 *  */
void mqtt_publish_task(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int time_gap = (int)pvParameters;

	char* mac = get_mac_address();

	if (!mac) {
		printf("Failed to get MAC address\n");
		vTaskDelete(NULL);
	}

	char topic[50];
	snprintf(topic, sizeof(topic), "agh/iot/%s/data", mac);

	for(;;) {
		const struct measurement measurement = get_measurement();
		char *message = convert_measurement_to_json_string(measurement);

		if (message) {
			mqtt_publish(topic, message);
			free(message);  // Free the memory allocated by convert_measurement_to_json_string
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000 * time_gap));  // 1000 ms = 1 second
	}

	free(mac);
}

/*Description:
 * Task for resetting the device
 *
 * @Input: [void*] pvParameters: none
 *
 * @Output: [void] Resets the device
 * */
void reset_task(void *pvParameters) {
    printf("Resetting device ...\n");
    vTaskDelay(1000);
    esp_restart();
}
