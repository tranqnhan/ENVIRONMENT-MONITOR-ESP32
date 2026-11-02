#pragma once

#include "p_scd41.h"

#define HTTP_MESSAGE_SIZE 256

void http_client_init();
void http_client_send_sensor_data(const measurement_t *m);
void http_client_is_ready(void);