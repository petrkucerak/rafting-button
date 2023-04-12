#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define A nodes[0]
#define B nodes[1]

int main(int argc, char const *argv[])
{
   printf("The simulation has been started\n");

   // create game structure
   game_t *game = NULL;
   game = (game_t *)malloc(sizeof(game_t));
   if (game == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [game]\n");
      exit(EXIT_FAILURE);
   }
   game->deadline = 10000;
   game->time = 0;
   game->nodes_count = 2;

   // create nodes
   node_t **nodes = NULL;
   nodes = (node_t **)malloc(sizeof(node_t) * game->nodes_count);
   if (nodes == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory [nodes]\n");
   }

   A->status = MASTER;
   if (A->status == MASTER)
      printf("MASTER\n");

   while (game->deadline != game->time || !game->deadline) {

      // start new episode
      ++game->time;
   }

   // Deallocated
   free(game);
   game = NULL;

   return 0;
}
