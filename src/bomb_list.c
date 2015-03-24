/*
 * bomb_list.c
 *
 *  Created on: 22 avr. 2014
 *      Author: bwh
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <game.h>
#include <misc.h>
#include <bomb.h>
#include <bomb_list.h>

struct cell {
	struct cell * next;
	struct bomb * bomb;
};

struct bomb_list {
	struct cell * first;
};

struct bomb_list * bomb_list_init() {
	struct cell * cell = malloc(sizeof(*cell));
	struct bomb_list * bomb_list = malloc(sizeof(*bomb_list));

	if (!bomb_list || !cell)
		error("Memory error");

	cell->next = NULL;
	cell->bomb = NULL;
	bomb_list->first = cell;

	return bomb_list;
}

struct bomb_list * list_add(struct bomb_list * list, struct bomb * bomb) {
	assert(list);
	assert(bomb);

	struct cell * cell = malloc(sizeof(*cell));
	if (!cell)
		error("Memory error");

	cell->bomb = bomb;
	cell->next = list->first;

	list->first = cell;

	return list;
}

struct cell * list_get_first(struct bomb_list * list) {
	assert(list);
	return list->first;
}

struct bomb * list_get_bomb(struct cell * cell) {
	assert(cell);
	return cell->bomb;
}

struct cell * get_next_cell(struct cell * cell) {
	assert(cell);
	return cell->next;
}

void bomb_list_disp(struct bomb_list * list, struct game * game,
		struct map * map) {
	assert(list);

	struct cell * current = list_get_first(list);
	while (get_next_cell(current) != NULL) {
		if (current != NULL) {
			bomb_disp(map, game, list_get_bomb(current));
		}
		current = get_next_cell(current);
	}
}

void bomb_list_delay(struct bomb_list * list, Uint32 delay) {
	assert(list);

	struct cell * current = list_get_first(list);
	struct bomb * bomb;
	Uint32 new_time;

	while (get_next_cell(current) != NULL) {
		bomb = list_get_bomb(current);
		new_time = bomb_get_timer(bomb) + delay;
		bomb_change_time(bomb, new_time);

		current = get_next_cell(current);
	}
}

void bomb_list_free(struct bomb_list * list) {
	assert(list);

	struct cell * current = list_get_first(list);
	struct cell * temp;
	struct bomb * bomb;

	while (get_next_cell(current) != NULL) {
		bomb = list_get_bomb(current);
		temp = get_next_cell(current);
		free(bomb);
		free(current);

		current = temp;
	}

	free(list_get_bomb(current));
	free(current);
}
