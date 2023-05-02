#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N nodes[node_no]
#define A nodes[0]
#define B nodes[1]
#define C nodes[2]
#define MASTER_NO 0

// #define DEBUG
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
   game->deadline = 8 * 60 * 10000; // 8 min (max value is UINT64_MAX)
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
      nodes[i].queue_head = NULL;
      nodes[i].queue_tail = NULL;
      nodes[i].status = SLAVE;
      nodes[i].time = 0;
      nodes[i].latency = 0;
      nodes[i].time_speed = 100;
      nodes[i].is_first_setup = 1;
      for (uint8_t j = 0; j < BALANCER_SIZE; ++j) {
         nodes[i].balancer[j] = 0;
      }
   }

   // ****** CONFIG ******
   // config enviroment to the simulation
   A.status = MASTER;
   A.time = 0;
   A.is_first_setup = 0;
   // A.latency = 10;

   B.time = 13;
   // B.latency = 12;

   C.time = 21;
   // C.latency = 8;
   // ****** CONFIG ******

   while (game->deadline > game->time || !game->deadline) {

      // set rnd latency
      A.latency = get_rnd_between(8, 14); // latency is 1 - 6 ms
      B.latency = get_rnd_between(8, 14); // latency is 1 - 6 ms
      C.latency = get_rnd_between(8, 14); // latency is 1 - 6 ms

      // time incementation
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         ++nodes[i].time;
         // TODO: time speed control
         for (uint8_t j = 0; j < BALANCER_SIZE; ++j) {
            ++nodes[i].balancer[j];
         }
      }

      // process pipe
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         process_pipe(i);
      }

      // status machine
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         while (!is_queue_empty(i)) {
            message_t *message = pop_from_queue(i);
            // SLAVE OPERATION
            if (!is_node_master(i)) {
               switch (message->type) {
               case TIME:
                  // sent message with same content to source
                  send_message(message->content, TIME, message->source, i,
                               nodes[i].latency);
                  break;

               case TIME_RTT:
                  // set time and sent back ACK
                  // first sync, simply set
                  if (nodes[i].is_first_setup) {
                     nodes[i].time = message->content;
                     for (uint8_t j = 0; j < BALANCER_SIZE; ++j) {
                        nodes[i].balancer[j] = message->content;
                     }
                     nodes[i].is_first_setup = 0;
#ifdef DEBUG
                     printf("INFO [%d]: First set time on value %ld\n", i,
                            nodes[i].time);
#endif // DEBUG
                  } else {
                     // not first sync, set avg
#ifdef DEBUG
                     printf("INFO [%d]: Set time on value %ld from %ld\n", i,
                            (nodes[i].time + message->content) / 2,
                            nodes[i].time);
#endif // DEBUG

                     nodes[i].balancer[game->time % BALANCER_SIZE] =
                         message->content;
                     nodes[i].time = 0;
                     for (uint8_t j = 0; j < BALANCER_SIZE; ++j) {
                        nodes[i].time += nodes[i].balancer[j];
                     }
                     nodes[i].time /= BALANCER_SIZE;
#ifdef DEBUG
                     for (uint8_t j = 0; j < BALANCER_SIZE; ++j) {
                        printf(" %ld", nodes[i].balancer[j]);
                     }
                     printf("\n");
#endif // DEBUG
                  }
                  send_message(message->content, ACK, message->source, i,
                               nodes[i].latency);
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

                  uint64_t time_rtt =
                      nodes[i].time + ((nodes[i].time - message->content) / 2);

#ifdef DEBUG
                  printf("INFO [%d]: Calcul TIME_RTT, time: %ld, RTT: %ld, "
                         "will set %ld\n",
                         i, nodes[i].time,
                         (nodes[i].time - message->content) / 2, time_rtt);
#endif // DEBUG

                  // get RTT and sent TIME_RTT message
                  send_message(time_rtt, TIME_RTT, message->source, i,
                               nodes[i].latency);
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
      // sync starts each 50 ms from MATER node
      if (!(game->time % 500)) {
         for (uint8_t i = 1; i < game->nodes_count; ++i) {
            send_message(nodes[MASTER_NO].time, TIME, i, MASTER_NO,
                         nodes[i].latency);
         }
      }

      // print round report

      printf("%ld", A.time);
      for (uint8_t i = 1; i < game->nodes_count; ++i) {
         printf(",%lld", (long long int)(nodes[i].time - A.time));
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
void push_to_pipe(message_t *message, uint8_t source, uint8_t target,
                  uint32_t delay)
{
   pipe_t *new_pipe = (pipe_t *)malloc(sizeof(pipe_t));
   if (new_pipe == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [push_to_pipe]\n");
      exit(EXIT_FAILURE);
   }
   new_pipe->message = message;
   new_pipe->delay = delay + game->time;
   new_pipe->target = target;
   new_pipe->next = NULL;
   if (nodes[source].pipe_tail == NULL) {
      nodes[source].pipe_tail = new_pipe;
      nodes[source].pipe_head = new_pipe;
      return;
   }
   nodes[source].pipe_tail->next = new_pipe;
   nodes[source].pipe_tail = new_pipe;
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
void pop_from_pipe_to_queue(uint8_t node_no)
{
   if (N.pipe_head == NULL) {
      fprintf(stderr, "ERROR: Queue is empty [pop_from_pipe]\n");
      exit(EXIT_FAILURE);
   }
   pipe_t *tmp = N.pipe_head;
   N.pipe_head = N.pipe_head->next;

   if (N.pipe_head == NULL)
      N.pipe_tail = NULL;

   message_t *message = tmp->message;

   push_to_queue(message, tmp->target);
   free(tmp);
}

void process_pipe(uint8_t node_no)
{

   while (N.pipe_head != NULL && N.pipe_head->delay <= game->time) {
      pop_from_pipe_to_queue(node_no);
   }
}

void send_message(uint64_t content, message_type_t type, uint8_t target,
                  uint8_t source, uint64_t delay)
{
   message_t *message = (message_t *)malloc(sizeof(message_t));
   if (message == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [send_message]\n");
      exit(EXIT_FAILURE);
   }
   message->content = content;
   message->type = type;
   message->source = source;
   message->delay = game->time + delay;

   push_to_pipe(message, source, target, delay);
#ifdef DEBUG
   printf("INFO [%d]: Mesage type %d pushed to pipe with %ld to %d\n", source,
          type, content, target);
#endif // DEBUG
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

uint8_t is_after_delay(uint8_t node_no)
{
   if (game->time >= N.queue_head->message->delay)
      return 1;
   return 0;
}
uint32_t get_rnd_between(uint32_t min, uint32_t max)
{
   return (uint32_t)((rand() % (max - min + 1)) + min);
}