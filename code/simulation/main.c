#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define N nodes[node_no]

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
      nodes[i].queue_head = NULL;
      nodes[i].queue_tail = NULL;
      nodes[i].status = SLAVE;
      nodes[i].time = 0;
      nodes[i].time_speed = 100;
   }

   // TODO: test queue mechanism
   {
      message_t *test_message = NULL;
      test_message = (message_t *)malloc(sizeof(message_t));
      test_message->type = TIME;
      test_message->content = 64789;
      push_to_queue(test_message, 0);
   }
   {
      message_t *test_message = NULL;
      test_message = (message_t *)malloc(sizeof(message_t));
      test_message->type = TIME;
      test_message->content = 159;
      push_to_queue(test_message, 0);
   }
   {
      message_t *test_message = NULL;
      test_message = (message_t *)malloc(sizeof(message_t));
      test_message->type = TIME;
      test_message->content = 984638;
      push_to_queue(test_message, 0);
   }

   printf("%ld\n", nodes[0].queue_head->message->content);
   printf("%ld\n", nodes[0].queue_head->next->message->content);
   printf("%ld\n", nodes[0].queue_head->next->next->message->content);

   // message_t *test_message = pop_from_queue(0);
   // printf("%ld \n", test_message->content);
   // test_message = pop_from_queue(0);
   // printf("%ld \n", test_message->content);
   // test_message = pop_from_queue(0);
   // printf("%ld \n", test_message->content);

   // game mechanism

   // print report

   // clean memory

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
      fprintf(stderr, "ERROR: Queue is empyt [pop_from_queue]\n");
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