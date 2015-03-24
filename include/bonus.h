/*
 * bonus.h
 *
 *  Created on: 6 mai 2014
 *      Author: bwh
 */

#ifndef BONUS_H_
#define BONUS_H_

void bonus_drop(struct game * game, struct map *map, int x, int y);
void bonus_get(struct map * map, struct player * player, int x, int y);

#endif /* BONUS_H_ */
