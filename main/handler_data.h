#ifndef _HANDLER_DATA_H_
#define _HANDLER_DATA_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef void(*data_handler_function_callback)(char arr_send_mqtt[], uint32_t leng);
uint16_t change_arr_str_to_int(char arr_char[], uint8_t arr_int[]);
void function_handler_int_array(uint8_t arr_int[], uint32_t leng_arr_data, void(*data_handler_callback)(char arr_send_mqtt[], uint32_t leng));

#endif
