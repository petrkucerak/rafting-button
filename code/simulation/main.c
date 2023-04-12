#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define A nodes[0]
#define B nodes[1]

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
   game->deadline = 1000000;
   game->time = 0;
   game->nodes_count = 2;

   // create nodes
   nodes = NULL;
   nodes = (node_t *)malloc(sizeof(node_t) * game->nodes_count);
   if (nodes == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [nodes]\n");
   }

   A.status = MASTER;
   A.time = 0;
   A.time_speed = 100;

   B.status = SLAVE;
   B.time = 0;
   B.time_speed = 101;

   while (game->deadline != game->time || !game->deadline) {

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
