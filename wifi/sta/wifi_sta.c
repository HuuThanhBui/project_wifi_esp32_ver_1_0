#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_sta.h"
#include "esp_smartconfig.h"

// #define EXAMPLE_ESP_WIFI_SSID      "Thanh1996"
// #define EXAMPLE_ESP_WIFI_PASS      "10091999"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define ESPTOUCH_DONE_BIT  BIT3

static EventGroupHandle_t s_wifi_event_group;
TaskHandle_t Task_Handle_Sudently_Lost_Wifi_Control = NULL;
TaskHandle_t Task_Handle_Smartconfig = NULL;
TaskHandle_t Task_Handle_Manager = NULL;
static const char *TAG = "wifi station";
esp_err_t status_of_smartconfig = (-1);

static void function_handle_sudenly_lost_connect(void *pvParameters);
static void smartconfig_example_task(void * parm);

static void function_manager_task(void *pvParameters)
{
    int status = 0;
    
    information_connect_wifi_t *information_wifi = (information_connect_wifi_t *)pvParameters;
    while(1)
    {
        if(status == 0 && information_wifi->status_of_connect == 0)
        {
            printf("\nEntry_1\n");
            if(Task_Handle_Sudently_Lost_Wifi_Control == NULL){
                printf("\nEntry_1\n");
                if(status_of_smartconfig == ESP_OK) {esp_smartconfig_stop();}
                if(Task_Handle_Smartconfig != NULL){vTaskDelete(Task_Handle_Smartconfig);}
                Task_Handle_Smartconfig = NULL;
                vTaskDelay(1000/portTICK_PERIOD_MS);
                xTaskCreate(function_handle_sudenly_lost_connect, "NAME", (1024*8), (void*)information_wifi, 2, &Task_Handle_Sudently_Lost_Wifi_Control);
                vTaskDelay(1000/portTICK_PERIOD_MS);
            }
            status = 1;
        }
        else if(status == 1 && information_wifi->status_of_connect == 0)
        {
            printf("\nEntry_2\n");
            if(Task_Handle_Smartconfig == NULL){
                printf("\nEntry_2\n");
                esp_wifi_disconnect();
                if(Task_Handle_Sudently_Lost_Wifi_Control != NULL){vTaskDelete(Task_Handle_Sudently_Lost_Wifi_Control);}
                Task_Handle_Sudently_Lost_Wifi_Control = NULL;
                vTaskDelay(1000/portTICK_PERIOD_MS);
                xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, &Task_Handle_Smartconfig);
                vTaskDelay(1000/portTICK_PERIOD_MS);
            }
            status = 0;
        }
        vTaskDelay(25000/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static uint8_t sudenly_lost_connect = 0;
    information_connect_wifi_t *connect_wifi_t = (information_connect_wifi_t*)arg; 

    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        if (connect_wifi_t->s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) 
        {
            esp_wifi_connect();
            connect_wifi_t->s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } 
        else 
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            connect_wifi_t->status_of_connect = 0;
            // esp_wifi_connect();
        }
        ESP_LOGI(TAG,"connect to the AP fail");
        if(sudenly_lost_connect == 1)
        {
            connect_wifi_t->status_of_connect = 0;
            sudenly_lost_connect = 0;
            xTaskCreate(function_manager_task, "task_manager", 4096, (void*)connect_wifi_t, 3, &Task_Handle_Manager);
        }
    } 
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        connect_wifi_t->s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        connect_wifi_t->status_of_connect = 1;
        sudenly_lost_connect = 1;
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) 
    {
        ESP_LOGI(TAG, "Scan done");
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) 
    {
        ESP_LOGI(TAG, "Found channel");
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) 
    {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i=0; i<33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_connect();
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) 
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static void smartconfig_example_task(void * parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    status_of_smartconfig = esp_smartconfig_start(&cfg);
    ESP_ERROR_CHECK(status_of_smartconfig);
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if(uxBits & WIFI_CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

static void wifi_init(information_connect_wifi_t *information_connect_wifi, char ssid[], char pass[])
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        (void*)(information_connect_wifi),
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        (void*)(information_connect_wifi),
                                                        &instance_got_ip));
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, (void*)(information_connect_wifi)));

    wifi_config_t wifi_config = {
        .sta = {
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    strcpy((char*)wifi_config.sta.ssid,ssid);
    strcpy((char*)wifi_config.sta.password,pass);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ssid, pass);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ssid, pass);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

static void function_handle_sudenly_lost_connect(void *pvParameters)
{
    information_connect_wifi_t *information_wifi = (information_connect_wifi_t *)pvParameters;
    
    while(1)
    {
        printf("\n..................");
        printf("\n..................");
        printf("\n..................");
        printf("\nLost wifi sudently !!!!!");
        printf("\n..................");
        printf("\n..................");
        esp_wifi_connect();
        if(information_wifi->status_of_connect == 1)
        {
            printf("\n.....");
            printf("\n-------------------------------------- Connect wifi success -----------------------------------\n");
            // free(information_wifi);
            vTaskDelete(NULL);
        }
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

information_connect_wifi_t *wifi_init_station(char ssid[], char pass[])
{
    information_connect_wifi_t *information_wifi;
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    information_wifi = (information_connect_wifi_t *)malloc(sizeof(information_wifi));
    information_wifi->status_of_connect = 0;
    information_wifi->s_retry_num = 0;
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init(information_wifi,ssid,pass);
    if(information_wifi->s_retry_num >= 5)
    {
        printf(".....");
        printf("\n-------------------------------------- Connect wifi failed -----------------------------------\n");
        // free(information_wifi);
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
        
        return information_wifi;
    }
    else
    {
        printf("\n.....");
        printf("\n-------------------------------------- Connect wifi success -----------------------------------\n");
        // free(information_wifi);
        return information_wifi;
    }
    return NULL;
}

