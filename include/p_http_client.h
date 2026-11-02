#pragma once

#include "p_scd41.h"

#define HTTP_MESSAGE_SIZE 256

void http_client_init(const char *server_url);
void http_client_send_sensor_data(    
    const measurement_t *m, 
    const char *measured_at_utc, 
    const char *device_name
);