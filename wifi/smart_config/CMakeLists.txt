set(app_src wifi_smart_config.c)
set(pri_req )
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req}
                    PRIV_REQUIRES nvs_flash wpa_supplicant
                    )

