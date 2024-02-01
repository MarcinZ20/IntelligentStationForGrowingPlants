#include "include/wifi.h"

/*Description:
 * Function that performs http GET request for given parameters
 *
 * @Input:
 *  - [const char*] host: web host
 *  - [const char*] path: web path
 *
 * @Output:
 *  - [void] Prints response to terminal
 *  */
void http_get(const char *host, const char *path) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    // Get the server's IP address
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Build the HTTP GET request
    char request[MAX_BUFFER_SIZE];
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);

    // Send the request
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error sending request");
        exit(EXIT_FAILURE);
    }

    // Receive and print the response
    char response[MAX_BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        printf("%s", response);
    }

    if (bytes_received < 0) {
        perror("Error receiving response");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
}

/*Description:
 * Event handler for Wi-Fi events
 *
 * @Input:
 *  - [void*] arg: ___
 *  - [esp_event_base_t] event_base: ___
 *  - [int32_t] event_id: id of the incoming event
 *  - [void*] event_data: data of the incoming event
 *
 * @Output:
 *  - [void] Only handles events
 * */
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wifi started!");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
	        xEventGroupSetBits(s_wifi_event_group, WIFI_DISCONNECTED_BIT);
            ESP_LOGI(TAG, "Reconnecting to wifi ...");
            esp_wifi_connect();
            s_retry_num++;
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
	    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
	    ESP_LOGI(TAG, "Connected to ip: "
	    IPSTR, IP2STR(&event->ip_info.ip));
    }
    
}

/*Description:
 * Function to configure Wi-Fi module
 *
 * @Input: [void]
 *
 * @Output: [void] Configures Wi-Fi with set parameters
 * */
static void config_wifi(void) {

    s_wifi_event_group = xEventGroupCreate();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
            .sta = {
                    .ssid = ESP_WIFI_SSID,
                    .password = ESP_WIFI_PASS,
                    .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                    .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
                    .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
            },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
