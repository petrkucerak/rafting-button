#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#define BALANCER_SIZE_RTT 200
#define BALACNER_SIZE_RTT_2 10
#define BALACNER_SIZE_DEVIATION 15

// SIMULATION TIME
// - targeted delay is between 1 ms - 10 ms
// - global time 1 step is 1 µs
// - local time (f=240MHz => T=4.16e-9) so 1 step represents 250 ticks

typedef enum status { MASTER, SLAVE } status_t;
typedef enum message_type { RTT_CAL, RTT_VAL, TIME } message_type_t;

typedef struct game {
   uint64_t time;       // auto-increment value, 1 step represents 1 µs
   uint64_t deadline;   // the 0 value means an infinite loop
   uint8_t nodes_count; // count of nodes
} game_t;

typedef struct message {
   message_type_t type;
   uint64_t content;
   uint8_t source;
   uint64_t delay;
} message_t;

typedef struct pipe {
   message_t *message;
   struct pipe *next;
   uint8_t target;
   uint32_t delay;
} pipe_t;

typedef struct queue {
   message_t *message;
   struct queue *next;
} queue_t;

typedef struct print_tmp {
   int32_t time;
   int32_t rtt;
   int32_t latency;
} print_tmp_t;

typedef struct node {
   uint64_t time; // auto incement, 1 step represents 250 procesor ticks
   uint32_t balancer_RTT[BALANCER_SIZE_RTT];            // latency
   int64_t balancer_deviation[BALACNER_SIZE_DEVIATION]; // deviation
   status_t status;                                     // MASTER or SLAVE
   uint8_t
       time_speed; // frequency deviation of less than ±10 ppm
                   // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#overview
                   // if timer 80MHz => one tick is 12.5 ns ± 0.124 ps deviation
                   // if 1 step is 1 µs, timer do 80 ticks in one step, so
                   // the max deviation is 80*0.124 ps = 0.00992 ns ~= 0.01 ns
                   // => max processor deviation is 0.01 ns / 1 µs (1 step)
                   // => cpu drift shut be one per 1 000 steps
                   // time_speed variable randomize deviation
   uint32_t latency; // assume that one message cannot overtake the other
                     // 1 represents 1 µs
   queue_t *queue_head;
   queue_t *queue_tail;
   pipe_t *pipe_head;
   pipe_t *pipe_tail;
   uint8_t is_first_setup_rtt;
   uint8_t is_first_setup_deviation;
   uint32_t stamp_rtt;
   uint32_t stamp_devition;
   int64_t deviation_abs;
   int64_t deviation_last_tmp;
   int64_t deviation_abs_2;
   int64_t deviation_last_tmp_2;
   print_tmp_t *last_printed_values;
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
                  uint32_t delay);
void pop_from_pipe_to_queue(uint8_t node_no);
void process_pipe(uint8_t node_no);

void send_message(uint64_t content, message_type_t type, uint8_t target,
                  uint8_t source, uint64_t delay);

uint8_t is_queue_empty(uint8_t node_no);
uint8_t is_node_master(uint8_t node_no);
uint32_t get_rnd_between(uint32_t min, uint32_t max);
uint32_t get_rtt_abs(uint8_t node_no);
uint32_t get_rtt_abs_limit(uint8_t node_no);
int64_t get_deviation_abs(uint8_t node_no);
int64_t get_deviation_last(uint8_t node_no);

#endif // MAIN_H