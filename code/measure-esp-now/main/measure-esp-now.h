#ifndef MEASURE_ESP_NOW_H
#define MEASURE_ESP_NOW_H

#include "esp_now.h"

/**
 * @brief Measures esp-now latency by sending broadcast messages and logging
 * round-time trip.
 *
 */
void measure_broadcast_task(void);

#endif // MEASURE_ESP_NOW_H