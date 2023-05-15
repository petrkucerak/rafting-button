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
   uint8_t source;      // index of sender node
   uint64_t latency;    // message latency
} message_t;

/**
 * @brief The pipe structure is used to realize latency. The structure is
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
   uint8_t is_first_setup_rtt;       // represents if RTT is set first time
   uint8_t is_first_setup_deviation; // represents if deviation is set first
   uint32_t rtt_index;               // actual rtt index
   int64_t deviation_avg;            // average deviation (O~)
   print_tmp_t last_print;           // structure with last printed data
} node_t;

/**
 * @brief The function pushed a message into the queue that stores messages
 * incoming into the node.
 *
 * @param message
 * @param node_no node index number
 */
void push_to_queue(message_t *message, uint8_t node_no);

/**
 * @brief The function popped a message from a queue that stores messages
 * incoming into the node.
 *
 * @param node_no node index number
 * @return message_t*
 */
message_t *pop_from_queue(uint8_t node_no);

/**
 * @brief The function does priority push a message into the pipe that is used
 * to realize latency.
 *
 * @param message
 * @param source index of sender node
 * @param target index of target node
 * @param latency latency length
 */
void push_to_pipe(message_t *message, uint8_t source, uint8_t target,
                  uint32_t latency);

/**
 * @brief The function popped a message from a pipe that is used to realize
 * latency.
 *
 * @param node_no node index number
 */
void pop_from_pipe_to_queue(uint8_t node_no);

/**
 * @brief The function popped messages in the pipe queue that reach a latency
 * limit.
 *
 * @param node_no node index number
 */
void process_pipe(uint8_t node_no);

/**
 * @brief The function does send the message. It means to push into the pipe
 * queue, where the message waits until latency reaches the limits. After that,
 * the message is pushed into the queue as an incoming message into the target
 * node.
 *
 * @param content content represent time in this simulation only
 * @param type type of message
 * @param target index of target node
 * @param source index of source node
 * @param latency latency length
 */
void send_message(uint64_t content, message_type_t type, uint8_t target,
                  uint8_t source, uint64_t latency);

/**
 * @brief The function returns a boolean value that expresses if is the queue
 * empty. This function is used for starting pop action.
 *
 * @param node_no node index number
 * @return uint8_t
 */
uint8_t is_queue_empty(uint8_t node_no);

/**
 * @brief The function returns a boolean value that expresses if the node has
 * master status.
 *
 * @param node_no node index number
 * @return uint8_t
 */
uint8_t is_node_master(uint8_t node_no);

/**
 * @brief Get the rnd between two values
 *
 * @param min min values is 0
 * @param max max value is UINT32_MAX
 * @return uint32_t
 */
uint32_t get_rnd_between(uint32_t min, uint32_t max);

/**
 * @brief Get the rnd between two values with two modes by percent.
 * Distribution minor > splitter > major
 *
 * @param min_major
 * @param max_major
 * @param min_minor
 * @param max_minor
 * @param splitter 0 - 100
 * @return uint32_t
 */
uint32_t get_rnd_between_with_distribution(uint32_t min_major,
                                           uint32_t max_major,
                                           uint32_t min_minor,
                                           uint32_t max_minor,
                                           uint8_t splitter);

/**
 * @brief Get the average of RTT in current node
 *
 * @param node_no node index number
 * @return uint32_t
 */
uint32_t get_rtt_avg(uint8_t node_no);

#endif // MAIN_H