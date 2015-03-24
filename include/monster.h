/*
 * monster.h
 *
 *  Created on: 3 mai 2014
 *      Author: bwh
 */

#ifndef MONSTER_H_
#define MONSTER_H_

#include <SDL/SDL.h>
#include <game.h>
struct monster;

struct monster * monster_init(int x, int y);
void monster_free(struct monster * monster);
int monster_get_x(struct monster * monster);
int monster_get_y(struct monster * monster);
void monster_set_pos(struct monster * monster, int x, int y);
int monster_is_alive(struct monster * monster);
void monster_set_current_way(struct monster * monster, enum way way);
int monster_move(struct game * game, struct monster* monster, struct map* map);
void monster_disp(struct monster* monster);
void monster_kill(struct monster * monster, struct map * map);

#endif /* MONSTER_H_ */
