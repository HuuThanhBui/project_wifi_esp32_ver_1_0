#ifndef _WIFI_STA_H_
#define _WIFI_STA_H_

typedef struct information_connect_wifi information_connect_wifi_t;
struct information_connect_wifi
{
    uint8_t s_retry_num;
    uint8_t status_of_connect;
}; 

information_connect_wifi_t *wifi_init_station(char ssid[], char pass[]);

#endif
