#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define A nodes[0]
#define B nodes[1]
#define A_ADDR 0
#define B_ADDR 1

node_t *nodes;
game_t *game;

int main(int argc, char const *argv[])
{
   printf("The simulation has been started\n");

   // create game structure
   game = NULL;
   game = (game_t *)malloc(sizeof(game_t));
   if (game == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [game]\n");
      exit(EXIT_FAILURE);
   }
   game->deadline = 1000;
   game->time = 0;
   game->nodes_count = 2;

   // create nodes
   nodes = NULL;
   nodes = (node_t *)malloc(sizeof(node_t) * game->nodes_count);
   if (nodes == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [nodes]\n");
      exit(EXIT_FAILURE);
   }

   A.status = MASTER;
   A.time = 0;
   A.time_speed = 100;
   A.queue = NULL;

   B.status = SLAVE;
   B.time = 0;
   B.time_speed = 101;
   B.queue = NULL;

   while (game->deadline != game->time || !game->deadline) {

      // ACTION TRIGGER
      if (game->time % 100 == 0) {
         // reading actions
         if (!is_queue_empty(B_ADDR)) {
            printf("NOW_CHCI\n");
            message_t *mess = queue_pop(nodes[B_ADDR].queue);
            switch (mess->type) {
            case TIME:
               printf("type: TIME, content: %ld\n", mess->content);
               break;
            case TIME_RTT:
               printf("type: TIME_RTT, content: %ld\n", mess->content);
               break;
            case ACK:
               printf("type: ACK, content: %ld\n", mess->content);
               break;
            }
         }

         // sending actions
         send_message(TIME, A.time, B_ADDR);
         if (nodes[B_ADDR].queue == NULL) {
            // printf("NOW\n");
         }
      }

      // RUTINES
      // game rutine
      ++game->time;

      // nodes rutine
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         nodes[i].time += nodes[i].time_speed;
      }
   }

   // Final report
   printf("\n\n");
   printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   printf("|           THE FINAL REPORT           |\n");
   printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   for (uint8_t i = 0; i < game->nodes_count; ++i) {
      if (nodes[i].status == MASTER)
         printf("|∘ NODE %2d | time %20ld |\n", i, nodes[i].time);
      else
         printf("|  NODE %2d | time %20ld |\n", i, nodes[i].time);
   }
   printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

   // Deallocated
   free(game);
   game = NULL;
   free(nodes);
   nodes = NULL;

   return 0;
}

void queue_push(uint8_t node, message_t *message)
{
   queue_t *queue = nodes[node].queue;
   if (queue != NULL) {
      while (queue->next != NULL) {
         queue = queue->next;
      }
      queue = queue->next;
   }
   queue = (queue_t*)malloc(sizeof(queue_t));
   if(queue == NULL){
      fprintf(stderr, "ERROR: Can't allocated memory [queue_push]\n");
      exit(EXIT_FAILURE);
   }
   queue->next = NULL;
   queue->message = &message;
   
   
   // printf("%ld\n", queue->message->content);
   if(queue == NULL){
      printf("NOW\n");
   }
}
message_t *queue_pop(queue_t *queue)
{
   queue_t *tmp = queue;
   queue = queue->next;
   tmp->next = NULL;
   message_t *ret = tmp->message;
   free(tmp);
   tmp = NULL;
   return ret;
}

void send_message(message_type_t type, uint64_t content, uint8_t target_node)
{
   message_t *message = NULL;
   message = (message_t *)malloc(sizeof(message_t));
   if (message == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [send_message]\n");
      exit(EXIT_FAILURE);
   }
   message->content = content;
   message->type = type;

   // send the message into the queue
   queue_push(target_node, message);
   // printf("%ld\n", nodes[target_node].queue->message->content);
}

uint8_t is_queue_empty(uint8_t node)
{
   if (nodes[node].queue == NULL)
      return 1;
   else
      return 0;
}