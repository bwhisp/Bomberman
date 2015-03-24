/*
 * monster_list.c
 *
 *  Created on: 8 mai 2014
 *      Author: bwh
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <misc.h>
#include <monster.h>
#include <monster_list.h>

struct cell {
	struct cell * next;
	struct monster * monster;
};

struct monster_list {
	struct cell * first;
};

struct monster_list * monster_list_init() {
	struct cell * cell = malloc(sizeof(*cell));
	struct monster_list * monster_list = malloc(sizeof(*monster_list));

	if (!monster_list || !cell)
		error("Memory error");

	cell->next = NULL;
	cell->monster = NULL;
	monster_list->first = cell;

	return monster_list;
}

struct monster_list * m_list_add(struct monster_list * list,
		struct monster * monster) {
	assert(list);
	assert(monster);

	struct cell * cell = malloc(sizeof(*cell));
	if (!cell)
		error("Memory error");

	cell->monster = monster;
	cell->next = list->first;

	list->first = cell;

	return list;
}

struct cell * m_list_get_first(struct monster_list * list) {
	assert(list);
	return list->first;
}

struct monster * list_get_monster(struct cell * cell) {
	assert(cell);
	return cell->monster;
}

struct cell * m_get_next_cell(struct cell * cell) {
	assert(cell);
	return cell->next;
}

void monster_list_disp(struct monster_list * list, struct game * game,
		struct map * map) {
	assert(list);

	struct cell * current = m_list_get_first(list);
	while (m_get_next_cell(current) != NULL) {
		monster_disp(list_get_monster(current));
		current = m_get_next_cell(current);
	}
}

void monster_from_map(struct monster_list* list, struct map* map) {
	assert(list);
	assert(map);

	int x, y;
	for (x = 0; x < map_get_width(map); x++) {
		for (y = 0; y < map_get_height(map); y++) {
			if (map_get_cell_type(map, x, y) == CELL_MONSTER) {
				m_list_add(list, monster_init(x, y));
			}
		}
	}
}

void monster_list_display(struct monster_list * list) {
	assert(list);

	struct cell * current = m_list_get_first(list);
	struct monster * monster;

	while (m_get_next_cell(current) != NULL) {
		monster = list_get_monster(current);
		monster_disp(monster);
		current = m_get_next_cell(current);
	}
}

void monster_list_move(struct game * game, struct monster_list * list,
		struct map * map) {
	assert(list);

	struct cell * current = m_list_get_first(list);
	struct monster * monster;
	int decision;

	while (m_get_next_cell(current) != NULL) {
		monster = list_get_monster(current);
		decision = randomize(1, 4);
		if (monster_is_alive(monster)) {
			switch (decision) {
			case 1:
				monster_set_current_way(monster, NORTH);
				break;
			case 2:
				monster_set_current_way(monster, SOUTH);
				break;
			case 3:
				monster_set_current_way(monster, WEST);
				break;
			case 4:
				monster_set_current_way(monster, EAST);
				break;
			}
			monster_move(game, monster, map);
		}
		current = m_get_next_cell(current);

	}
}

struct monster * monster_search(struct monster_list * list, int x, int y) {
	assert(list);

	struct cell * current = m_list_get_first(list);
	struct monster * monster;

	while (m_get_next_cell(current) != NULL) {
		monster = list_get_monster(current);

		if (monster_get_x(monster) == x && monster_get_y(monster) == y)
			return monster;

		current = m_get_next_cell(current);
	}
	return NULL;
}

void monster_list_free(struct monster_list * list) {
	assert(list);

	struct cell * current = m_list_get_first(list);
	struct cell * temp;
	struct monster * monster;
	while (m_get_next_cell(current) != NULL) {
		monster = list_get_monster(current);
		temp = m_get_next_cell(current);

		free(monster);
		free(current);

		current = temp;
	}

	free(list_get_monster(current));
	free(current);

}
