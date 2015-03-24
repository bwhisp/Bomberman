#ifndef GAME_H_
#define GAME_H_

#include <player.h>
#include <level.h>
#include <SDL/SDL.h>
#include <SDL/SDL_stdinc.h>

struct game;
int win;

// Create a new game
struct game* game_new();

// Sets new lists
void game_set_monster_list(struct game * game);
void game_set_bomb_list(struct game * game);

// Free a game
void game_free(struct game* game);

// Return the player of the current game
struct player* game_get_player(struct game* game);

// Return the current level
struct level* game_get_curr_level(struct game* game);

//Return the bomb list
struct bomb_list * game_get_bomb_list(struct game * game);
struct monster_list * game_get_monster_list(struct game * game);

// Display the game of the screen
void game_display(struct game* game);

// update
int game_update(struct game* game);

// Special moves
int game_update(struct game* game);
void game_special_c2(struct game *game);
int game_has_C2(struct game * game);

// Sets a curr_level to game
void game_set_level(struct game *game, struct level* level);

// Builds a struct game with the given arguments
void game_build(struct game * game, struct level *level, struct player * player,
		struct bomb_list * bombs, struct monster_list * monsters, int C2);


// Start and end screens
void start_screen();
void win_screen();
void lose_screen();

#endif /* GAME_H_ */
