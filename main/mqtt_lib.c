#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "mqtt_lib.h"

#define BROKER_URI  "mqtt://broker.hivemq.com/"
#define PORT        1883
#define CLIENT_ID   "Client_ID_Device_1"
#define TOPIC_PUBLIC    "project/main/public"
#define TOPIC_SUBCRI    "project/main/subcriber"

static mqtt_function_data_callback function_callback = NULL;

static esp_mqtt_client_handle_t client = NULL;

static const char *TAG = "MQTTS_EXAMPLE";

void mqtt_app_start(void)
{
    esp_mqtt_client_start(client);
}

void mqtt_pub(char *topic, char *data, int len)
{
    esp_mqtt_client_publish(client, topic, data, len, 1, 0);
}

void mqtt_sub(char *topic)
{
    esp_mqtt_client_subscribe(client, topic, 1);
}

void mqtt_deinit(void)
{
    if (client == NULL)
    {
        return;
    }
    esp_mqtt_client_stop(client);
    esp_mqtt_client_disconnect(client);
    esp_mqtt_client_destroy(client);
    client = NULL;
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    char arr_data[500] = {0};
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_sub(TOPIC_SUBCRI);
            printf("\nHave been subcribed attribute topic !!!\n");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
        {
            memset(arr_data, 0, sizeof(arr_data));
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            // printf("\nData: %s  -- length: %d\n",event->data,event->data_len);
            snprintf(arr_data, (event->data_len + 1), "%s", event->data);
            function_callback(arr_data, event->data_len);
            break;
        }
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_init(void(*function_data_callback)(char data[], uint32_t leng))
{
    if(function_data_callback != NULL)
    {
        function_callback = function_data_callback;
    }
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = BROKER_URI,            
        .port = PORT,
        .event_handle = mqtt_event_handler,
        .client_id = CLIENT_ID,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    mqtt_app_start();
}





