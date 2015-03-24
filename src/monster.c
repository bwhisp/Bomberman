/*
 * monster.c
 *
 *  Created on: 3 mai 2014
 *      Author: bwh
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <misc.h>
#include <map.h>
#include <monster.h>
#include <window.h>
#include <sprite.h>

struct monster {
	int x, y;
	enum way current_way;
	int alive;
};

struct monster * monster_init(int x, int y) {
	struct monster * monster = malloc(sizeof(*monster));
	if (!monster)
		error("Memory error");

	monster->current_way = SOUTH;
	monster->alive = 1;

	monster->x = x;
	monster->y = y;

	return monster;
}

void monster_free(struct monster * monster) {
	assert(monster);
	free(monster);
}

int monster_get_x(struct monster * monster) {
	assert(monster != NULL);
	return monster->x;
}

int monster_get_y(struct monster * monster) {
	assert(monster != NULL);
	return monster->y;
}

void monster_set_pos(struct monster * monster, int x, int y) {
	assert(monster != NULL);
	monster->x = x;
	monster->y = y;
}

int monster_is_alive(struct monster * monster) {
	assert(monster != NULL);
	return monster->alive;
}

void monster_kill(struct monster * monster, struct map * map) {
	assert(monster !=NULL);

	int x = monster_get_x(monster);
	int y = monster_get_y(monster);

	monster->alive = 0;
	monster->x = -2;
	monster->y = -2;

	map_set_cell_type(map, x, y, CELL_EMPTY);
}

enum way monster_get_current_way(struct monster * monster) {
	assert(monster != NULL);
	return monster->current_way;
}

void monster_set_current_way(struct monster * monster, enum way way) {
	assert(monster);
	monster->current_way = way;
}

static int monster_move_aux(struct monster* monster, struct map* map, int x,
		int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_CASE:
		return 0;
		break;

	case CELL_BONUS:
		return 0;
		break;

	case CELL_GOAL:
		return 0;
		break;

	case CELL_MONSTER:
		return 1;
		break;

	case CELL_BOMB:
		return 1;
		break;

	case CELL_PLAYER:
		return 2;
		break;

	case CELL_KEY:
		return 0;
		break;

	case CELL_DOOR:
		return 0;
		break;

	case CELL_CLOSED_DOOR:
		return 0;
		break;

	case CELL_FLAG:
		return 0;
		break;

	default:
		break;
	}
	// Monster has moved
	return 1;
}

int monster_move(struct game * game, struct monster* monster, struct map* map) {
	struct player * player = game_get_player(game);
	int x = monster->x;
	int y = monster->y;
	int move = 0;
	switch (monster->current_way) {
	case NORTH:
		if (monster_move_aux(monster, map, x, y - 1) == 2
				&& !player_is_god(player)) {
			player_wound(game_get_player(game));
		}
		if (monster_move_aux(monster, map, x, y - 1)) {
			monster->y--;
			move = 1;
		}

		break;

	case SOUTH:
		if (monster_move_aux(monster, map, x, y + 1) == 2) {
			player_wound(game_get_player(game));
		}
		if (monster_move_aux(monster, map, x, y + 1)) {
			monster->y++;
			move = 1;
		}
		break;

	case WEST:
		if (monster_move_aux(monster, map, x - 1, y) == 2) {
			player_wound(game_get_player(game));
		}
		if (monster_move_aux(monster, map, x - 1, y)) {
			monster->x--;
			move = 1;
		}
		break;

	case EAST:
		if (monster_move_aux(monster, map, x + 1, y) == 2) {
			player_wound(game_get_player(game));
		}
		if (monster_move_aux(monster, map, x + 1, y)) {
			monster->x++;
			move = 1;
		}
		break;
	}

	if (move) {
		map_set_cell_type(map, x, y, CELL_EMPTY);
		if (map_get_cell_type(map, monster->x, monster->y) == CELL_BOMB) {
			switch (map_get_cell_complete_type(map, monster->x, monster->y)) {
			case CELL_BOMB_1:
				map_set_cell_type(map, monster->x, monster->y, CELL_M_BOMB_1);
				break;
			case CELL_BOMB_2:
				map_set_cell_type(map, monster->x, monster->y, CELL_M_BOMB_2);
				break;
			case CELL_BOMB_3:
				map_set_cell_type(map, monster->x, monster->y, CELL_M_BOMB_3);
				break;
			case CELL_BOMB_4:
				map_set_cell_type(map, monster->x, monster->y, CELL_M_BOMB_4);
				break;
			case CELL_BOMB_EXPLODED:
				map_set_cell_type(map, monster->x, monster->y,
						CELL_M_BOMB_EXPLODED);
				break;
			default:
				break;
			}
		} else {
			map_set_cell_type(map, monster->x, monster->y, CELL_MONSTER);
		}
	}

	return move;
}

void monster_disp(struct monster * monster) {
	assert(monster);
	if (monster_is_alive(monster))
		window_display_image(sprite_get_monster(monster->current_way),
				monster->x * SIZE_BLOC, monster->y * SIZE_BLOC);
}
