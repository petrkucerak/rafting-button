#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N nodes[node_no]
#define A nodes[0]
#define B nodes[1]
#define C nodes[2]
#define D nodes[3]
#define MASTER_NO 0

#define N_106 1000000
#define N_102 100
#define N_101 10

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
   game->deadline = 3 * 60 * 1000000; // x min (max value is UINT64_MAX)
   // game->deadline = 1000000; // x min (max value is UINT64_MAX)
   game->nodes_count = 4;
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
      nodes[i].pipe_head = NULL;
      nodes[i].pipe_tail = NULL;
      nodes[i].status = SLAVE;
      nodes[i].time = 0;
      nodes[i].latency = 0;
      nodes[i].time_speed = 0;
      nodes[i].is_first_setup_rtt = 1;
      nodes[i].is_first_setup_deviation = 1;
      nodes[i].stamp_rtt = 0;
      nodes[i].stamp_devition = 0;
      nodes[i].deviation_abs = 0;
      nodes[i].deviation_last_tmp = 0;
      nodes[i].deviation_abs_2 = 0;
      nodes[i].deviation_last_tmp_2 = 0;
      nodes[i].last_print.latency = 0;
      nodes[i].last_print.rtt = 0;
      nodes[i].last_print.time = 0;
      for (uint8_t j = 0; j < BALANCER_SIZE_RTT; ++j) {
         nodes[i].balancer_RTT[j] = 0;
      }
      for (uint8_t j = 0; j < BALACNER_SIZE_DEVIATION; ++j) {
         nodes[i].balancer_deviation[j] = 0;
      }
   }

   // ****** CONFIG ******
   // config enviroment to the simulation
   A.status = MASTER;
   A.time = 0;
   A.is_first_setup_rtt = 0;
   // A.time_speed = get_rnd_between(1, 15);
   A.time_speed = 1;

   B.time = get_rnd_between(100, 1500);
   // B.time_speed = get_rnd_between(1, 15);
   B.time_speed = 0;

   C.time = get_rnd_between(100, 1500);
   C.time_speed = 0;

   D.time = get_rnd_between(100, 1500);
   D.time_speed = 0;

   // ****** CONFIG ******

   while (game->deadline > game->time || !game->deadline) {

      // set rnd latency
      A.latency = get_rnd_between(900, 1500); // latency is 0.8 - 1.4 ms
      B.latency = get_rnd_between(900, 1500); // latency is 0.8 - 1.4 ms
      C.latency = get_rnd_between(900, 1500); // latency is 0.8 - 1.4 ms
      D.latency = get_rnd_between(900, 1500);

      // time incementation
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         ++nodes[i].time;

         // create timer deviation, more infromation is in main.h
         // if value is 0, no deviation is applied
         if (nodes[i].time_speed != 0) {
            if (!(game->time % (1000 * nodes[i].time_speed))) {
               if (get_rnd_between(0, 1))
                  ++nodes[i].time;
               else
                  --nodes[i].time;
            }
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
               case RTT_CAL:
                  // send value back
                  send_message(message->content, RTT_CAL, message->source, i,
                               nodes[i].latency);
                  break;
               case RTT_VAL:
                  // Save RTT value
                  // for first message, paste rtt into all array
                  if (nodes[i].is_first_setup_rtt) {
                     for (uint32_t j = 0; j < BALANCER_SIZE_RTT; ++j) {
                        nodes[i].balancer_RTT[j] = message->content;
                     }
                     nodes[i].is_first_setup_rtt = 0;
                  } else {
                     nodes[i].balancer_RTT[nodes[i].stamp_rtt] =
                         message->content;

                     // Increase the stamp value
                     ++nodes[i].stamp_rtt;
                     if (nodes[i].stamp_rtt == BALANCER_SIZE_RTT)
                        nodes[i].stamp_rtt = 0;
                  }

#ifdef DEBUG
                  {
                     printf("# RND ABS [%d]: %d ||", i, get_rtt_abs(i));
                     for (uint32_t j = 0; j < BALANCER_SIZE_RTT; ++j) {
                        printf(" %d", nodes[i].balancer_RTT[j]);
                     }
                     printf("\n");
                  }
#endif // DEBUG
                  break;

               case TIME:
                  // Calcule deviation 0
                  //          D1
                  // T1 ----------------> T2
                  // T2 = T1 + D1 + O1
                  // 01 = T2 - T1 - D1, in reality use only average =>
                  // 0~ = T2 - T1 - D~

                  break;

               default:
                  fprintf(stderr, "ERROR: Unknown operation\n");
                  exit(EXIT_FAILURE);
                  break;
               }
            } else {

               // MASTER OPERATION
               switch (message->type) {
               case RTT_CAL:
                  // calcule and send RTT to slave
                  send_message(((nodes[i].time - message->content) / 2),
                               RTT_VAL, message->source, i, nodes[i].latency);
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

      // Start RTT synchronization
      // sync starts each 100 ms from MATER node
      if (!(game->time % 100000)) {
         for (uint8_t i = 1; i < game->nodes_count; ++i) {
            send_message(nodes[MASTER_NO].time, RTT_CAL, i, MASTER_NO,
                         nodes[i].latency);
         }
      }

      // Start TIME sychronization
      // sync starts each 500 ms from MASTER node
      if ((game->time % 500000) == 499999) {
         for (uint8_t i = 1; i < game->nodes_count; ++i) {
            send_message(nodes[MASTER_NO].time, TIME, i, MASTER_NO,
                         nodes[i].latency);
         }
      }

#ifdef BUILD_REPORT
      // print round report
      int print_changed = 0;
      for (uint8_t i = 0; i < game->nodes_count; ++i) {
         if (get_rtt_abs(i) != nodes[i].last_print.rtt) {
            print_changed = 1;
         }
         nodes[i].last_print.rtt = get_rtt_abs(i);
      }

      if (print_changed == 1 || A.time == 1) {
         printf("%ld", A.time);
         for (uint8_t i = 1; i < game->nodes_count; ++i) {
            printf(",%d", get_rtt_abs(i));
         }
         printf("\n");
      }

#endif // BUILD_REPORT

      // increment game round id
      ++game->time;
   }

   // clear memory
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

   // If a queue is an empty
   if (nodes[source].pipe_tail == NULL) {
      nodes[source].pipe_tail = new_pipe;
      nodes[source].pipe_head = new_pipe;
      return;
   }

   // Special Case: The head of list has lesser priority than new node. So
   // insert new node before head node and change head node.
   if (nodes[source].pipe_head->delay > new_pipe->delay) {
      // Insert new pipe element before the head
      new_pipe->next = nodes[source].pipe_head;
      nodes[source].pipe_head = new_pipe;
      return;
   }

   pipe_t *tmp = NULL;
   tmp = nodes[source].pipe_head;
   // Traverse the list and find a position to insert new node
   while (tmp->next != NULL && tmp->next->delay < new_pipe->delay) {
      tmp = tmp->next;
   }
   // Either at the ends of the list or at required position
   new_pipe->next = tmp->next;
   tmp->next = new_pipe;
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

uint32_t get_rtt_abs(uint8_t node_no)
{
   uint32_t rtt = 0;
   for (uint8_t i = 0; i < BALANCER_SIZE_RTT; ++i) {
      rtt += N.balancer_RTT[i];
   }
   return (uint32_t)((rtt / BALANCER_SIZE_RTT));
}

uint32_t get_rtt_abs_limit(uint8_t node_no)
{
   uint32_t rtt = 0;
   for (uint8_t i = 0; i < BALACNER_SIZE_RTT_2; ++i) {
      int stamp = N.stamp_rtt - i;
      if (stamp < 0)
         stamp += BALANCER_SIZE_RTT;
      rtt += N.balancer_RTT[stamp];
   }
   return (uint32_t)(rtt / BALACNER_SIZE_RTT_2);
}

int64_t get_deviation_abs(uint8_t node_no)
{
   int64_t devition = 0;
   for (uint8_t i = 0; i < BALACNER_SIZE_DEVIATION; ++i) {
      devition += N.balancer_deviation[i];
   }
   return (int64_t)(devition / BALACNER_SIZE_DEVIATION);
}

int64_t get_deviation_last(uint8_t node_no)
{
   if (N.stamp_devition == 0)
      return (int64_t)N.balancer_deviation[BALACNER_SIZE_DEVIATION - 1];
   else
      return (int64_t)N.balancer_deviation[N.stamp_devition - 1];
}