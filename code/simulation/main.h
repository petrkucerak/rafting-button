#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

typedef enum status { MASTER, SLAVE } status_t;

typedef struct game {
   uint64_t time;       // auto-increment value
   uint64_t deadline;   // the 0 value means an infinite loop
   uint8_t nodes_count; // count of nodes
} game_t;

typedef struct node {
   uint64_t time;    // local time
   status_t status;  // MASTER or SLAVE
   float time_speed; // control speed of local time
} node_t;

#endif // MAIN_H