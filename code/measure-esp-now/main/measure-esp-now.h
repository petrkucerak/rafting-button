#ifndef MEASURE_ESP_NOW_H
#define MEASURE_ESP_NOW_H

#include "esp_now.h"

/**
 * @brief Measures esp-now latency by sending broadcast messages and logging
 * round-time trip.
 *
 */
void measure_broadcast_task_master(void);
void measure_broadcast_task_slave(void);
void measure_unicast_task_master(void);

#endif // MEASURE_ESP_NOW_H