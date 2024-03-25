#ifndef _UART_LIB_H_
#define _UART_LIB_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef enum{
    uart0,
    uart2
}uart_name;

typedef enum{
    en_odd,
    en_even,
    dis_parity
}uart_parity;

typedef enum{
    bit_5,
    bit_6,
    bit_7,
    bit_8
}uart_size_data;

typedef enum{
    en,
    dis_flow,
    en_rts,
    en_cts
}uart_flow_control;

typedef struct uart_data uart_data_t;
struct uart_data{
    uart_name name;
    uint8_t pin_tx;
    uint8_t pin_rx;
    uint32_t baudrate;
    uart_size_data uart_frame_size; 
    uart_parity parity;
    uart_flow_control flow_control;
    void(*data_function_callback)(char data[], uint32_t size_payload, uart_data_t *uart_data);
};

typedef void(*variable_temp_func_callback)(char data[], uint32_t size_payload, uart_data_t *uart_data);

void uart_lib_init(uart_data_t uart_data);
void uart_send(char data[], uint32_t len, uart_name name);

#endif
