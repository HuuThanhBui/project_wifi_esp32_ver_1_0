set(app_src wifi_ap.c)
set(pri_req spi_flash esp_http_server spiffs nvs_flash esp_http_client json)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})

