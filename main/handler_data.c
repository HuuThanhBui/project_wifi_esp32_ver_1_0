#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "handler_data.h"

#define START_BUFFER        0xB1
#define LENG_BUFF_SENSOR    7
#define LENG_BUFF_LED       6

static data_handler_function_callback function_callback = NULL;

uint16_t change_arr_str_to_int(char arr_char[], uint8_t arr_int[])
{
	char str_tok[3] = {0};
    uint8_t j = 0;
    int data = 0;
    uint16_t z = 0;
    
    for(uint32_t i = 0; i < strlen(arr_char); i++)
    {
        if(arr_char[i] == ' ' || arr_char[i] == '\0' || arr_char[i] == '\n')
        {
            str_tok[j] = '\0';
            sscanf(str_tok, "%x",&data);
            arr_int[z] = data;
            memset(str_tok, 0, sizeof(str_tok));
            j = 0;
            z++;
        }
        else
        {
            str_tok[j] = arr_char[i];
            j++;
        }
    }
    return z;
}

void function_handler_int_array(uint8_t arr_int[], uint32_t leng_arr_data, void(*data_handler_callback)(char arr_send_mqtt[], uint32_t leng))
{
    uint8_t leng_data = 0;
    char data_send_to_mqtt[200] = {0};
    if(data_handler_callback != NULL)
    {
        function_callback = data_handler_callback;
    }
    for(uint32_t i = 0; i < leng_arr_data; i++)
    {
        if(arr_int[i] == START_BUFFER)
        {
            leng_data = arr_int[i+1];
            if(leng_data == LENG_BUFF_SENSOR)
            {
                sprintf(data_send_to_mqtt,"Type_Device: %d Zone: 0x%X CMD_ID: 0x%X Ope_Hand: 0x%X Data_1: 0x%X Data_2: 0x%X", arr_int[0], arr_int[i+2], arr_int[i+3], arr_int[i+4], arr_int[i+5], arr_int[i+6]);
                // printf("Data_Send_To_MQTT: %s\n",data_send_to_mqtt);
                function_callback(data_send_to_mqtt, strlen(data_send_to_mqtt));
            }
            else if(leng_data == LENG_BUFF_LED)
            {
                sprintf(data_send_to_mqtt,"Type_Device: %d Zone: 0x%X CMD_ID: 0x%X Ope_Hand: 0x%X Data: 0x%X", arr_int[0],arr_int[i+2], arr_int[i+3], arr_int[i+4], arr_int[i+5]);
                // printf("Data_Send_To_MQTT: %s\n",data_send_to_mqtt);
                function_callback(data_send_to_mqtt, strlen(data_send_to_mqtt));
            }
        }
    }
}





