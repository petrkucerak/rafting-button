#include "main.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
   printf("The simulation has been started\n");

   game_t *game = NULL;
   game = (game_t *)malloc(sizeof(game_t));
   if (game == NULL) {
      fprintf(stderr, "ERROR: Can't allocated memory\n");
      exit(EXIT_FAILURE);
   }
   game->deadline = 10000;
   game->time = 0;

   while (game->deadline != game->time || !game->deadline) {

      // start new episode
      ++game->time;
   }

   return 0;
}
