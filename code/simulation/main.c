#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define N nodes[node_no]
#define A nodes[0]
#define B nodes[1]
#define C nodes[2]
#define MASTER_NO 0

#define BUILD_REPORT

game_t *game;
node_t *nodes;

int main(int argc, char const *argv[])
{

   // create resources
   game = NULL;
   game = (game_t *)malloc(sizeof(game_t));
   if (game == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [game_t]\n");
      exit(EXIT_FAILURE);
   }

   // ****** CONFIG ******
   // set up game parametrs
   game->deadline = 10000000; // max value is UINT64_MAX
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
      nodes[i].time_speed = 100;
   }

   // ****** CONFIG ******
   // config enviroment to the simulation
   A.status = MASTER;
   A.time_speed = 250;

   B.time = 2489;
   B.time_speed = 198;

   C.time = 987467;
   C.time_speed = 2;
   // ****** CONFIG ******

   // TODO: game mechanism

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
            if (!is_queue_empty(i)) {
               message_t *message = pop_from_queue(i);
               switch (message->type) {
               case TIME:
                  // send time back
                  // printf("Slave TIME\n");
                  send_message(message->content, TIME, message->source, i);
                  break;
               case TIME_RTT:
                  // set TIME and sent ACK back
                  // set TIME: if differnt > x set, if differnt < x set average
                  // different is 10 μs => different 2500 (2500 * 4 ns)
                  // set means set (TIME + RTT) * 1.5 -> 3 ways
                  // printf("Slave RTT + TIME\n");
                  if (message->content > 2500) {
                     nodes[i].time = message->content;
                  } else {
                     nodes[i].time = ((message->content) + nodes[i].time) / 2;
                  }
                  send_message(0, ACK, MASTER_NO, i);
                  break;
               }
               free(message);
               message = NULL;
            }
         }
         // MASTER operations
         if (!is_queue_empty(MASTER_NO)) {
            message_t *message = pop_from_queue(MASTER_NO);
            switch (message->type) {
            case TIME:
               // send TIME with RTT
               // printf("Master TIME\n");
               uint64_t RTT = (A.time - message->content) / 2;
               send_message(A.time + RTT, TIME_RTT, message->source, MASTER_NO);
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

// print round report
#ifdef BUILD_REPORT
      printf("%ld", nodes[0].time);
      for (uint8_t i = 1; i < game->nodes_count; ++i) {
         printf(",%ld", nodes[i].time);
      }
      printf("\n");
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