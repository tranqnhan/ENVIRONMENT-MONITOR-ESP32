#pragma once

#define MAX_PROVISION_TIME_SECS 60

void environment_monitor_task_measure(void *pvParameter);
void environment_monitor_task_wifi_provision(void *pvParameter);