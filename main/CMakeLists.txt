idf_component_register(SRCS "app_main.c" "mqtt_lib.c" "uart_lib.c" "handler_data.c"
                    INCLUDE_DIRS "." "../wifi/sta/" "../wifi/smart_config/")

# idf_component_register(SRCS "app_main.c" "sensor_lib.c" "sim7600.c" "mqtt_lib.c"
# INCLUDE_DIRS "." "../wifi/sta/" "../wifi/smart_config/")