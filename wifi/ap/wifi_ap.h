#ifndef _WIFI_AP_H_
#define _WIFI_AP_H_

// void start_webserver(void);
// void stop_webserver(void *arg);

typedef void (*http_post_handle_t) (char *data, int len);
void start_webserver(void);
void stop_webserver(void);
void http_post_set_callback(void *cb);

#endif
