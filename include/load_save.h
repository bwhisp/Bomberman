/*
 * load_save.h
 *
 *  Created on: 3 mai 2014
 *      Author: bwh
 */

#ifndef LOAD_SAVE_H_
#define LOAD_SAVE_H_

#include <game.h>

struct map * load_map(int map_num);
int save_game(struct game * game);
void load_game(struct game * game);

#endif /* LOAD_SAVE_H_ */
