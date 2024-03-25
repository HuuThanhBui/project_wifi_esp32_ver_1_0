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
#include "uart_lib.h"

// #define TXD2 17
// #define RXD2 16

static variable_temp_func_callback variable_func_callback = NULL;

void uart_event_task(void *arg)
{
    uart_data_t *uart_data = (uart_data_t*)arg;
    uint32_t rxBytes = 0;
    char* data = (char*) malloc((1024)+1);
    char bufData[1024] = {0};

    while(1)
    {
        if(uart_data->name == uart0)
        {
            rxBytes = uart_read_bytes(UART_NUM_0, data, 1024, 500/portTICK_RATE_MS);
        }
        else if(uart_data->name == uart2)
        {
            rxBytes = uart_read_bytes(UART_NUM_2, data, 1024, 500/portTICK_RATE_MS);
        }
            
        if(rxBytes > 0)
        {
            memset(bufData, 0, sizeof(bufData));
            // char *bufData = calloc(rxBytes + 1, sizeof(data));
            snprintf(bufData, rxBytes + 1, "%s", (char *)data);
            // printf("Data nhan duoc: %s  ---  size: %d\n",data,rxBytes);
            variable_func_callback(bufData, rxBytes, uart_data);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    free(data);
    vTaskDelete(NULL);
}

void uart_lib_init(uart_data_t uart_data)
{
    uart_config_t uart_config;
    QueueHandle_t uart_queue;
    uart_config.baud_rate = uart_data.baudrate;
    if(uart_data.uart_frame_size == bit_5)
    {
        uart_config.data_bits = UART_DATA_5_BITS;
    }
    else if(uart_data.uart_frame_size == bit_6)
    {
        uart_config.data_bits = UART_DATA_6_BITS;
    }
    else if(uart_data.uart_frame_size == bit_7)
    {
        uart_config.data_bits = UART_DATA_7_BITS;
    }
    else if(uart_data.uart_frame_size == bit_8)
    {
        uart_config.data_bits = UART_DATA_8_BITS;
    }

    if(uart_data.parity == en_odd)
    {
        uart_config.parity = UART_PARITY_ODD;
    }
    else if(uart_data.parity == en_even)
    {
        uart_config.parity = UART_PARITY_EVEN;
    }
    else if(uart_data.parity == dis_parity)
    {
        uart_config.parity = UART_PARITY_DISABLE;
    }

    uart_config.stop_bits = UART_STOP_BITS_1;

    if(uart_data.flow_control == en)
    {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
    }
    else if(uart_data.flow_control == dis_flow)
    {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    }
    else if(uart_data.flow_control == en_rts)
    {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
    }
    else if(uart_data.flow_control == en_cts)
    {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS;
    }

    uart_config.source_clk = UART_SCLK_APB;

    if(uart_data.data_function_callback != NULL)
    {
        variable_func_callback = uart_data.data_function_callback;
    }

    if(uart_data.name == uart0)
    {
        uart_driver_install(UART_NUM_0, (1024 * 2), (1024 * 2), 2048, &uart_queue, 0);
        uart_param_config(UART_NUM_0, &uart_config);
        uart_set_pin(UART_NUM_0,uart_data.pin_tx,uart_data.pin_rx,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    }
    else if(uart_data.name == uart2)
    {
        uart_driver_install(UART_NUM_2, (1024 * 2), (1024 * 2), 2048, &uart_queue, 0);
        uart_param_config(UART_NUM_2, &uart_config);
        uart_set_pin(UART_NUM_2,uart_data.pin_tx,uart_data.pin_rx,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    } 
    xTaskCreate(uart_event_task, "uart_event_task", 2048 * 4, (void *)&uart_data, 12, NULL);
}

void uart_send(char data[], uint32_t len, uart_name name)
{
    if(name == uart0)
    {
        uart_write_bytes(UART_NUM_0, (char*)data, len);
    }
    else if(name == uart2)
    {
        uart_write_bytes(UART_NUM_2, (char*)data, len);
    }
}





