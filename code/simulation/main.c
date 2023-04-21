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
   game->deadline = 60000; // (max value is UINT64_MAX)
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
      nodes[i].latency = 0;
      nodes[i].time_speed = 100;
      nodes[i].is_first_setup = 1;
   }

   // ****** CONFIG ******
   // config enviroment to the simulation
   A.status = MASTER;
   A.time = 0;
   A.is_first_setup = 0;

   B.time = 23529;
   C.time = 98021;
   // ****** CONFIG ******

   while (game->deadline > game->time || !game->deadline) {

      // time incementation
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         ++nodes[i].time;
      }

      // status machine
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         if (!is_queue_empty(i)) {
            message_t *message = pop_from_queue(i);
            // SLAVE OPERATION
            if (!is_node_master(i)) {
               switch (message->type) {
               case TIME:
                  // sent message with same content to source
                  send_message(message->content, TIME, message->source, i);
                  break;

               case TIME_RTT:
                  // set time and sent back ACK
                  // first sync, simply set
                  if (nodes[i].is_first_setup) {
                     nodes[i].time = message->content;
                     nodes[i].is_first_setup = 0;
                  } else {
                     // not first sync, set avg
                     nodes[i].time = (nodes[i].time + message->content) / 2;
                  }
                  send_message(message->content, ACK, message->source, i);
                  break;

               default:
                  fprintf(stderr, "ERROR: Unknown operation\n");
                  exit(EXIT_FAILURE);
                  break;
               }
            } else {
               // MASTER OPERATION
               switch (message->type) {
               case TIME:
                  // get RTT and sent TIME_RTT message
                  // time + rtt = (b - a) + b = 2b - a
                  // uint64_t time_rtt = 2 * nodes[i].time - message->content;
                  send_message(2 * nodes[i].time - message->content, TIME_RTT, message->source, i);
                  break;
               case ACK:
                  // do nothing
                  break;
               default:
                  fprintf(stderr, "ERROR: Unknown operation\n");
                  exit(EXIT_FAILURE);
                  break;
               }
            }
            free(message);
            message = NULL;
         }
      }

      // start time synchronization
      // sync starts each 10 ms from MATER node
      if (!(game->time % 100)) {
         for (uint8_t i = 1; i < game->nodes_count; ++i) {
            send_message(nodes[MASTER_NO].time, TIME, i, MASTER_NO);
         }
      }

      // print round report

      printf("%ld", game->time);
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         printf(",%lld", (long long int)(nodes[i].time - game->time));
      }
      printf("\n");

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

uint8_t is_node_master(uint8_t node_no)
{
   if (N.status == MASTER)
      return 1;
   return 0;
}