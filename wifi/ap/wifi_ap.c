#include "wifi_ap.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include <string.h>

static const char *TAG = "example";
static httpd_handle_t server = NULL;

static http_post_handle_t http_post_cb = NULL;

static esp_err_t http_get_handler(httpd_req_t *req)
{
    const char *webpage = "Bui Huu Thanh Send Test !!!!";
    httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t http_get = {
    .uri       = "/get",
    .method    = HTTP_GET,
    .handler   = http_get_handler,
    .user_ctx  = "Hello World!"
};

static esp_err_t http_post_handler(httpd_req_t *req)
{
    char buf[100];
    int data_len = req->content_len;
    httpd_req_recv(req, buf,data_len);
    ESP_LOGI(TAG, "Data recv: %.*s", data_len, buf);
    http_post_cb(buf, data_len);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_post = {
    .uri       = "/post",
    .method    = HTTP_POST,
    .handler   = http_post_handler,
    .user_ctx  = NULL
};

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &http_get);
        httpd_register_uri_handler(server, &http_post);
    }
}

void stop_webserver(void)
{
    httpd_stop(server);
}

void http_post_set_callback(void *cb)
{
    if(cb){
        http_post_cb = cb;
    }
}














































// #include <stdio.h>
// #include "sdkconfig.h"
// #include "stdint.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_system.h"
// #include "esp_spi_flash.h"
// #include "esp_sntp.h"
// #include "malloc.h"
// #include <sys/param.h>
// #include <esp_http_server.h>
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// // #include "esp_tls.h"
// #include "lwip/sockets.h"
// #include "lwip/dns.h"
// #include "lwip/netdb.h"
// #include "esp_http_client.h"
// #include "string.h"
// #include "esp_wifi.h"
// #include "cJSON.h"
// #include "wifi_ap.h"

// #define SIZEOF_BUFF_DATA 2000
// static char *TAG_HTTP = "Http-Server-Webserver";

// typedef struct queue queue_mem;
// struct queue{
//     xQueueHandle queue_t;
// };

// typedef struct value value_t;
// struct value{
//   char *str;
// };

// static void recive_data(xQueueHandle queue)
// {
//     value_t mem_par = {};
//     while(xQueueReceive(queue,&mem_par,5000 / portTICK_PERIOD_MS))
//     {
//         printf("\nString_Data: %s\n",mem_par.str);
//     }
// }

// static esp_err_t adder_get_control_device(httpd_req_t *req)
// {
//     queue_mem *queue = (queue_mem*)req->user_ctx;
//     char buf[SIZEOF_BUFF_DATA];
//     memset(buf,0,sizeof(buf));
//     int ret, remaining = req->content_len;
//     while (remaining > 0) {
//         if ((ret = httpd_req_recv(req, buf,MIN(remaining, sizeof(buf)))) <= 0) {
//             if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//                 continue;
//             }
//             return ESP_FAIL;
//         }
//         printf("\nData recive: %s\n",buf);
//         value_t mem_value = {
//             .str = buf
//         };
//         long ok = xQueueSend(queue->queue_t,&mem_value,1000/portTICK_PERIOD_MS);
//         if(ok);
//         else printf("failed to add message to queue\n");
//         recive_data(queue->queue_t);
//         break;
//     }
//     return ESP_OK;
// }

// static esp_err_t adder_data_send(httpd_req_t *req)
// {
//     const char *webpage = "Bui Huu Thanh Send Test !!!!";
//     httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
//     return ESP_OK;
// }

// void *open_webserver(void)
// {
//     queue_mem *queue_mem_t;
//     queue_mem_t = (queue_mem*)malloc(sizeof(queue_mem));
//     queue_mem_t->queue_t = xQueueCreate(100,sizeof(value_t));
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//     ESP_LOGI(TAG_HTTP, "Starting server on port: '%d'", config.server_port);
//     httpd_handle_t server;

//     httpd_uri_t adder_get_control_led = {
//         .uri      = "/recive",
//         .method   = HTTP_GET,
//         .handler  = adder_get_control_device,
//         .user_ctx = (void*)queue_mem_t
//     };

//     httpd_uri_t adder_send_data = {
//         .uri      = "/send",
//         .method   = HTTP_GET,
//         .handler  = adder_data_send,
//         .user_ctx = NULL
//     };

//     if (httpd_start(&server, &config) == ESP_OK) 
//     {
//       ESP_LOGI(TAG_HTTP, "Registering URI handlers");
//       httpd_register_uri_handler(server, &adder_get_control_led);
//       httpd_register_uri_handler(server, &adder_send_data);
//       return (void*)server;
//     }

//     ESP_LOGI(TAG_HTTP, "Error starting server!");
//     return NULL;
// }

// void start_webserver(void)
// {
//     static httpd_handle_t server = NULL;
//     server = open_webserver();
// }

// void stop_webserver(void *arg)
// {
//     // Stop the httpd server
//     httpd_stop((httpd_handle_t)arg);
// }


