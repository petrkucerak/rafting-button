#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N nodes[node_no]
#define A nodes[0]
#define B nodes[1]
#define C nodes[2]
#define MASTER_NO 0

#define BUILD_REPORT

game_t *game;
node_t *nodes;

long long int log_tmp[3];
uint32_t generated_rnd[3];

int main(int argc, char const *argv[])
{

   // for rnd generator
   srand(time(NULL));

   // create resources
   game = NULL;
   game = (game_t *)malloc(sizeof(game_t));
   if (game == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [game_t]\n");
      exit(EXIT_FAILURE);
   }

   // ****** CONFIG ******
   // set up game parametrs
   game->deadline = 2000000000; // in ns (max value is UINT64_MAX)
   game->nodes_count = 3;
   // ****** CONFIG ******

   game->time = 0;
   nodes = NULL;
   nodes = (node_t *)malloc(sizeof(node_t) * game->nodes_count);
   if (nodes == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [nodes_t]\n");
      exit(EXIT_FAILURE);
   }
   for (uint8_t i = 0; i < game->nodes_count; ++i) {
      nodes[i].queue_head = NULL;
      nodes[i].queue_tail = NULL;
      nodes[i].status = SLAVE;
      nodes[i].time = 0;
      nodes[i].latency_min = 0;
      nodes[i].latency_max = 0;
      nodes[i].time_speed = 100;
   }

   // ****** CONFIG ******
   // config enviroment to the simulation
   A.status = MASTER;
   A.time_speed = 250;
   A.latency_min = 1;
   A.latency_max = 10;

   B.time = 67189;
   B.time_speed = 240;
   B.latency_min = 1;
   B.latency_max = 10;

   C.time = 147189;
   C.time_speed = 255;
   C.latency_min = 1;
   C.latency_max = 10;
   // ****** CONFIG ******

   log_tmp[0] = 0;
   log_tmp[1] = 0;
   log_tmp[2] = 0;
   generated_rnd[0] = 0;
   generated_rnd[1] = 0;
   generated_rnd[2] = 0;

#ifndef BUILD_REPORT
   printf("THE GAME HAS BEEN STARTED\n");
#endif // BUILD_REPORT

   while (game->deadline > game->time || !game->deadline) {
      // processors tick is each 4 ns
      if (!(game->time % 4)) {

         // CLOCK (local time)
         for (uint8_t i = 0; i < game->nodes_count; ++i) {
            ++nodes[i].time;
            // speed_time each n * 100 tick
            if (nodes[i].time_speed != 0 &&
                !(nodes[i].time % (nodes[i].time_speed * 100)))
               ++nodes[i].time;
         }

         // SLAVE operations
         for (uint8_t i = 0; i < game->nodes_count; ++i) {
            if (!generated_rnd[i])
               generated_rnd[i] = get_rnd_latency(i);
            if (!(game->time % generated_rnd[i])) {
               if (!is_queue_empty(i)) {
                  generated_rnd[i] = 0;
                  message_t *message = pop_from_queue(i);
                  switch (message->type) {
                  case TIME:
                     // send time back
                     // printf("Slave TIME\n");
                     send_message(message->content, TIME, message->source, i);
                     break;
                  case TIME_RTT:
                     // set TIME and sent ACK back
                     // set TIME: if differnt > x set, if differnt < x set
                     // average different is 10 μs => different 2500 (2500 * 4
                     // ns) set means set (TIME + RTT) * 1.5 -> 3 ways
                     // printf("Slave RTT + TIME\n");
                     if (message->content > 100) {
                        nodes[i].time = message->content;
                     } else {
                        nodes[i].time =
                            ((message->content) + nodes[i].time) / 2;
                     }
                     send_message(0, ACK, MASTER_NO, i);
                     break;
                  }
                  free(message);
                  message = NULL;
               }
            }
         }
         // MASTER operations
         if (!generated_rnd[MASTER_NO])
            generated_rnd[MASTER_NO] = get_rnd_latency(MASTER_NO);
         if (!(game->time % generated_rnd[MASTER_NO])) {
            if (!is_queue_empty(MASTER_NO)) {
               generated_rnd[MASTER_NO] = 0;
               message_t *message = pop_from_queue(MASTER_NO);
               switch (message->type) {
               case TIME:
                  // send TIME with RTT
                  // printf("Master TIME\n");
                  uint64_t RTT = (A.time - message->content) / 2;
                  send_message(A.time + RTT, TIME_RTT, message->source,
                               MASTER_NO);
                  break;
               case ACK:
                  // sucess sichnizoation
                  // printf("Master ACK\n");
                  break;
               }
               free(message);
               message = NULL;
            }
            // send time sych message each (4 ns * 10000 =) 40 μs
            if (!(A.time % 10000)) {
               // printf("Send\n");
               for (uint8_t i = 1; i < game->nodes_count; ++i) {
                  send_message(A.time, TIME, i, MASTER_NO);
               }
            }
         }
      }

// print round report
#ifdef BUILD_REPORT
      // A report
      // printf("%ld", nodes[0].time);
      // for (uint8_t i = 1; i < game->nodes_count; ++i) {
      //    printf(",%ld", nodes[i].time);
      // }
      // printf("\n");
      if ((long long int)(nodes[1].time - nodes[0].time) != log_tmp[1] ||
          (long long int)(nodes[2].time - nodes[0].time) != log_tmp[2]) {

         log_tmp[0] = (long long int)game->time;
         log_tmp[1] = (long long int)(nodes[1].time - nodes[0].time);
         log_tmp[2] = (long long int)(nodes[2].time - nodes[0].time);

         printf("%ld", game->time);
         printf(",%lld", log_tmp[1]);
         printf(",%lld\n", log_tmp[2]);
      }
#endif // BUILD_REPORT

      // increment game round id
      ++game->time;
   }

   // clean memory
   free(nodes);
   nodes = NULL;
   free(game);
   game = NULL;

   return 0;
}

void push_to_queue(message_t *message, uint8_t node_no)
{

   queue_t *new_queue = (queue_t *)malloc(sizeof(queue_t));
   if (new_queue == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [push_to_queue]\n");
      exit(EXIT_FAILURE);
   }
   new_queue->message = message;
   new_queue->next = NULL;

   if (N.queue_tail == NULL) {
      N.queue_tail = new_queue;
      N.queue_head = new_queue;
      return;
   }
   N.queue_tail->next = new_queue;
   N.queue_tail = new_queue;
}
message_t *pop_from_queue(uint8_t node_no)
{
   if (N.queue_head == NULL) {
      fprintf(stderr, "ERROR: Queue is empty [pop_from_queue]\n");
      exit(EXIT_FAILURE);
   }
   queue_t *tmp = N.queue_head;
   N.queue_head = N.queue_head->next;

   if (N.queue_head == NULL)
      N.queue_tail = NULL;

   message_t *message = tmp->message;
   free(tmp);
   return message;
}

void send_message(uint64_t content, message_type_t type, uint8_t target,
                  uint8_t source)
{
   message_t *message = (message_t *)malloc(sizeof(message_t));
   if (message == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [send_message]\n");
      exit(EXIT_FAILURE);
   }
   message->content = content;
   message->type = type;
   message->source = source;

   push_to_queue(message, target);
}

uint8_t is_queue_empty(uint8_t node_no)
{
   if (N.queue_tail == NULL)
      return 1;
   return 0;
}

uint32_t get_rnd_latency(uint8_t node_no)
{
   return (rand() % (N.latency_max - N.latency_min + 1)) + N.latency_min;
}