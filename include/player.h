#ifndef PLAYER_H_
#define PLAYER_H_

#include <map.h>
#include <constant.h>
#include <SDL/SDL.h>

struct player;
struct player_pos {
	int x;
	int y;
};

struct player_pos last_pos;

// Creates a new player with a given number of available bombs
struct player* player_init(int bomb_number, int range, int life, int keys);
void player_free(struct player* player);


// Returns the current position of the player
int player_get_x(struct player* player);
int player_get_y(struct player* player);


//Return info about bomb
int player_get_range(struct player* player);
int is_bomb(struct player* player);
void player_bomb_unset(struct player* player);
void player_bomb_set(struct player* player);


// Set the direction of the next move of the player
void player_set_current_way(struct player* player, way_t way);

// Infos about keys
int player_get_keys(struct player* player);
void player_inc_key (struct player * player);
void player_dec_key (struct player * player);

// Modifies bomb attributes
int player_get_nb_bomb(struct player* player);
void player_inc_nb_bomb(struct player* player);
void player_dec_nb_bomb(struct player* player);
void player_inc_range(struct player* player);
void player_dec_range(struct player* player);

// Gets,Increases, Decreases the life of the player
void player_wound(struct player * player);
void player_heal(struct player * player) ;
int player_get_life(struct player* player);
int player_is_god(struct player * player);
void player_unset_god(struct player * player);
Uint32 player_get_god_time (struct player * player);
enum way player_get_curr_way(struct player * player);

// Load the player position from the map
void player_from_map(struct player* player, struct map* map);

// Move the player according to the current direction
int player_move(struct player* player, struct map* map);

// Display the player on the screen
void player_display(struct player* player);


#endif /* PLAYER_H_ */
