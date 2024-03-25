#ifndef _MQTT_LIB_H_
#define _MQTT_LIB_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef void(*mqtt_function_data_callback)(char data[], uint32_t leng);
void mqtt_init(void(*function_data_callback)(char data[], uint32_t leng));
void mqtt_pub(char *topic, char *data, int len);
void mqtt_sub(char *topic);
void mqtt_deinit(void);

#endif
