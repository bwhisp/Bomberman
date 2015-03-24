/*
 * bomb.h
 *
 *  Created on: 8 mars 2014
 *      Authors: AbderKammous-47 & The Black Whisper
 */

#ifndef BOMB_H_
#define BOMB_H_

#include <player.h>
#include <misc.h>
#include <map.h>
#include <game.h>

struct bomb;

enum bomb_state {
	state_1 = 0,
	state_2,
	state_3,
	state_4,
	state_explosion,
	state_6
};

//Initialise a bomb
struct bomb * bomb_set (int x, int y, int range);
void bomb_map_set (struct player* player, struct map* map);

//Explode a bomb
void bomb_explode (struct game * game, struct map* map, struct bomb* bomb);
void bomb_explode_disp (struct map* map, struct bomb* bomb);

//Get infos
int bomb_get_timer(struct bomb* bomb);
int bomb_get_x(struct bomb* bomb);
int bomb_get_y(struct bomb* bomb);
int bomb_get_range(struct bomb* bomb);

//Display bombs
void bomb_disp(struct map* map, struct game * game, struct bomb* bomb);

//To change infos about a bomb
void bomb_change_state(struct bomb* bomb, enum bomb_state state);
void bomb_change_time(struct bomb* bomb, Uint32 time);

#endif /* BOMB_H_ */
