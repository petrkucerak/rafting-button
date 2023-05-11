/**
 * @file main.h
 * @author Petr Kucera (kucerp28@fel.cvut.cz)
 * @brief The file contains struct and function definitions for the simulation
 * file. More information about the simulation is in the repository:
 * https://github.com/petrkucerak/rafting-button
 *
 * This program aims to simulate an algorithm for synchronization time in the
 * distributed system.
 *
 * ## SIMULATION TIME
 * - 1 step is 1 µs
 * - CPU speed (ESP32) f=240MHz => T=4.16e-9, so 1 step represents 250 processor
 * ticks
 * - The processor has frequency deviation of less than ±10 ppm
 * (https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#overview).
 * If timer is 80 MHz => one tick is 12.5 ns ± 0.124 ps deviation, so if 1 step
 * is 1 µs, the timer does 80 ticks in one step, so the max deviation is
 * 80*0.124 ps = 0.00992 ns ~= 0.01 ns. It means that max processor deviation is
 * 0.01 ns / 1 µs (1 step) => CPU drift shut be one per 1 000 steps cpu_drift
 * variable randomize deviation ±1.
 *
 * @version 1.0
 * @date 2023-05-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#define BALANCER_SIZE_RTT 100

/**
 * @brief Node mode in DS
 *
 */
typedef enum status {
   MASTER, // a node that has the reference time
   SLAVE   // a node that has time synchronized by the master node
} status_t;

/**
 * @brief The type of message
 *
 */
typedef enum message_type {
   RTT_CAL, // time used to calculate the RTT
   RTT_VAL, // calculated value of RTT
   TIME     // time to synchronize the time
} message_type_t;

/**
 * @brief Structure to config the simulation
 *
 */
typedef struct simulation {
   uint64_t time;       // auto-increment value, 1 step represents 1 µs
   uint64_t deadline;   // the 0 value means an infinite loop
   uint8_t nodes_count; // count of nodes
} simulation_t;

/**
 * @brief Structure with message parameters
 *
 */
typedef struct message {
   message_type_t type; // type of message
   uint64_t content;    // content represent time in this simulation only
   uint8_t source;      // sender of message
   uint64_t latency;    // message latency
} message_t;

/**
 * @brief The pipe structure is used to realize the latency. The structure is
 * used as an element in the queue.
 *
 */
typedef struct pipe {
   message_t *message; // message
   struct pipe *next;  // next element in the queue
   uint8_t target;     // target of message
   uint32_t latency;   // message latency
} pipe_t;

/**
 * @brief The queue structure is used to store incoming messages for each node.
 * The structure is used as an element in the queue.
 *
 */
typedef struct queue {
   message_t *message; // message
   struct queue *next; // next element in the queue
} queue_t;

/**
 * @brief The structure is used for storing the last printed data. It makes the
 * printing process more data-saving because prevents creating duplicates.
 *
 */
typedef struct print_tmp {
   int32_t time;      // time difference between the master and slave node time
   int32_t rtt;       // RTT average (R~)
   int32_t deviation; // size of deviation (0~)
} print_tmp_t;

/**
 * @brief Represents each node
 *
 */
typedef struct node {
   uint64_t time; // auto-incement value in µs, 1 step == 250 procesor ticks
   uint32_t balancer_RTT[BALANCER_SIZE_RTT]; // array of RTT
   status_t status;                          // status of node
   uint8_t cpu_drift;                        // value of CPU drif
   uint32_t latency;                         // latency of sending message
   queue_t *queue_head; // pointer to queue header with receiving messages
   queue_t *queue_tail; // pointer to queue tail with receiving messages
   pipe_t *pipe_head; // pointer to the pipe header used for processing latency
   pipe_t *pipe_tail; // pointer to the pipe header used for processing latency
   uint8_t is_first_setup_rtt; // boolean value, represent if is first setting
                               // up the RTT
   uint8_t is_first_setup_deviation;
   uint32_t stamp_rtt;
   int64_t deviation_abs;
   print_tmp_t last_print;
} node_t;

/**
 * Normal queue
 */
void push_to_queue(message_t *message, uint8_t node_no);
message_t *pop_from_queue(uint8_t node_no);

/**
 * Priority
 */
void push_to_pipe(message_t *message, uint8_t source, uint8_t target,
                  uint32_t latency);
void pop_from_pipe_to_queue(uint8_t node_no);
void process_pipe(uint8_t node_no);

void send_message(uint64_t content, message_type_t type, uint8_t target,
                  uint8_t source, uint64_t latency);

uint8_t is_queue_empty(uint8_t node_no);
uint8_t is_node_master(uint8_t node_no);
uint32_t get_rnd_between(uint32_t min, uint32_t max);
uint32_t get_rtt_abs(uint8_t node_no);
uint32_t get_rtt_abs_limit(uint8_t node_no);
int64_t get_deviation_abs(uint8_t node_no);
int64_t get_deviation_last(uint8_t node_no);

#endif // MAIN_H