#ifndef MEASURE_ESP_NOW_H
#define MEASURE_ESP_NOW_H

/**
 * @brief The function measures esp-now latency by sending broadcast messages
 * and logging round-time trip.
 *
 */
void measure_broadcast();

/**
 * @brief The function initialize Wi-Fi for ESP-NOW
 *
 */
void wifi_init();

#endif // MEASURE_ESP_NOW_H