/*
 * bonus.c
 *
 *  Created on: 6 mai 2014
 *      Author: bwh
 */
#include <stdlib.h>
#include <stdio.h>

#include <map.h>
#include <player.h>
#include <monster_list.h>
#include <monster.h>
#include <game.h>
#include <misc.h>


void bonus_drop(struct game * game, struct map *map, int x, int y){

	int rand = randomize(0, 6);

	switch (rand) {
	case 0:
		map_set_cell_type(map, x, y, CELL_BONUS_BOMBINC);
		break;
	case 1:
		map_set_cell_type(map, x, y, CELL_BONUS_BOMBDEC);
		break;
	case 2:
		map_set_cell_type(map, x, y, CELL_BONUS_RANGEINC);
		break;
	case 3:
		map_set_cell_type(map, x, y, CELL_BONUS_RANGEDEC);
		break;
	case 4:
		map_set_cell_type(map, x, y, CELL_BONUS_LIFE);
		break;
	case 5:
		m_list_add(game_get_monster_list(game), monster_init(x, y));
		map_set_cell_type(map, x, y, CELL_MONSTER);
		break;
	case 6:
		map_set_cell_type(map, x, y, CELL_EMPTY);
		break;
	}
}

void bonus_get(struct map * map, struct player * player, int x, int y) {

	enum compose_type type = map_get_cell_complete_type(map, x, y);
	enum bonus_type bonus = type >> 4;
	if (map_get_cell_type(map, x, y) == CELL_BONUS) {
		switch (bonus) {
		case BONUS_BOMB_NB_INC:
			player_inc_nb_bomb(player);
			break;
		case BONUS_BOMB_NB_DEC:
			player_dec_nb_bomb(player);
			break;
		case BONUS_BOMB_RANGE_INC:
			player_inc_range(player);
			break;
		case BONUS_BOMB_RANGE_DEC:
			player_dec_range(player);
			break;
		case BONUS_LIFE:
			player_heal(player);
			break;
		default:
			break;
		}
	}
}

