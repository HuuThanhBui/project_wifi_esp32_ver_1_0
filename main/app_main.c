#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "../wifi/sta/wifi_sta.h"
#include "../wifi/ap/wifi_ap.h"
#include "../wifi/smart_config/wifi_smart_config.h"
#include "uart_lib.h"
#include "mqtt_lib.h"
#include "handler_data.h"

#define TXD2 17
#define RXD2 16
#define TOPIC_PUBLIC    "project/main/public"
#define TOPIC_SUBCRI    "project/main/subcriber"

void mqtt_data_function_callback(char data[], uint32_t leng)
{
    printf("Data mqtt: %s  ---  size: %d\n",data,leng);
    uart_send(data, leng, uart2);
    uart_send("\n", strlen("\n"), uart2);
}

void network_connect_task(void * parm)
{
    uint8_t flag_status = 0;
    information_connect_wifi_t *wifi_infor = (information_connect_wifi_t*)parm;
    while(1)
    {   
        printf("\nState of connect wifi: %d",wifi_infor->status_of_connect);
        if(wifi_infor->status_of_connect == 1)
        {
            if(flag_status == 0)
            {
                mqtt_init(mqtt_data_function_callback);
                flag_status = 1;
            }
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        else
        {
            // sim_send_mqtt_data(TELEMETRY_TOPIC,arr_temp,100,3);
            // vTaskDelay(300/portTICK_PERIOD_MS);
        }
        vTaskDelay(300/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void data_handler_callback_function(char arr_send_mqtt[], uint32_t leng)
{
    printf("Data after handler: %s  ---  size: %d\n",arr_send_mqtt,leng);
    mqtt_pub(TOPIC_PUBLIC, arr_send_mqtt, leng);
}

void function_uart_callback_handler(char data[], uint32_t size_payload, uart_data_t *uart_data)
{
    uint8_t arr_init[500] = {0}; 
    // printf("Data nhan duoc: %s  ---  size: %d\n",data,size_payload);
    change_arr_str_to_int(data, arr_init);
    function_handler_int_array(arr_init, (sizeof(arr_init)/sizeof(uint8_t)), data_handler_callback_function);
}

void app_main(void)
{
    information_connect_wifi_t *wifi_infor;

    // wifi_infor = wifi_init_station("R&DMOBIFONE_2.4GHz","Mobifone@2022");
    wifi_infor = wifi_init_station("Thanh1996","10091996");
    xTaskCreate(network_connect_task, "network_connect_task", (4096 * 4), (void *)wifi_infor, 3, NULL);

    uart_data_t uart_data;
    uart_data.name = uart2;
    uart_data.pin_tx = TXD2;
    uart_data.pin_rx = RXD2;
    uart_data.baudrate = 9600;
    uart_data.uart_frame_size = bit_8; 
    uart_data.parity = dis_parity;
    uart_data.flow_control = dis_flow;
    uart_data.data_function_callback = function_uart_callback_handler;

    uart_lib_init(uart_data);
    
    while(1)
    {
        // uart_send("ThanhBH_Test\n", strlen("ThanhBH_Test\n"), uart2);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
