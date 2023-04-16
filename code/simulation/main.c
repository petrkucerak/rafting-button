#include "main.h"
#include <stdio.h>
#include <stdlib.h>

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
   // set up game parametrs
   game->deadline = 10000;
   game->nodes_count = 2;
   game->time = 0;

   nodes = NULL;
   nodes = (node_t *)malloc(sizeof(node_t) * game->nodes_count);
   if (nodes == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [nodes_t]\n");
      exit(EXIT_FAILURE);
   }
   for (uint8_t i = 0; i < game->nodes_count; ++i) {
      nodes[i].queue = NULL;
      nodes[i].status = SLAVE;
      nodes[i].time = 0;
      nodes[i].time_speed = 100;
   }

   // TODO: test queue mechanism
   message_t *test_message = NULL;
   test_message = (message_t *)malloc(sizeof(message_t));
   test_message->type = TIME;
   test_message->content = 64789;
   push_to_queue(test_message, 0);

   printf("%ld \n", nodes[0].queue->message->content);
   test_message->content = 0;

   // game mechanism

   // print report

   // clean memory

   return 0;
}

void push_to_queue(message_t *message, uint8_t node_no)
{

   if (nodes[node_no].queue == NULL) {
      nodes[node_no].queue = (queue_t *)malloc(sizeof(queue_t));
      if (nodes[node_no].queue == NULL) {
         fprintf(stderr, "ERROR: Can't allocated memory [push_to_queue]\n");
         exit(EXIT_FAILURE);
      }
      nodes[node_no].queue->message = message;
      nodes[node_no].queue->next = NULL;
   }
}
message_t *pop_from_queue(uint8_t node_no)
{
   queue_t *queue = nodes[node_no].queue;
   if (queue == NULL) {
      fprintf(stderr, "ERROR: The queue is an empty\n");
      exit(EXIT_FAILURE);
   }
   nodes[node_no].queue = nodes[node_no].queue->next;
}