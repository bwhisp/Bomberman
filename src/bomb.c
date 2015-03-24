/*
 * bomb.c
 *
 *  Created on: 8 mars 2014
 *      Authors: AbderKammous-47 & The Black Whisper
 */

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <map.h>
#include <sprite.h>
#include <bomb.h>
#include <window.h>
#include <player.h>
#include <bonus.h>
#include <monster.h>
#include <monster_list.h>

struct bomb {
	int x;
	int y;
	int range;
	Uint32 time;
	enum bomb_state state;
};

struct bomb* bomb_set(int x, int y, int range) {

	struct bomb* bomb = malloc(sizeof(*bomb));
	if (!bomb)
		error("Memory error");

	bomb->range = range;
	bomb->time = SDL_GetTicks();
	bomb->x = x;
	bomb->y = y;

	bomb->state = state_4;

	return bomb;
}

//Sets the cell where the player is to a CELL_BOMB
void bomb_map_set(struct player* player, struct map* map) {
	int x = player_get_x(player);
	int y = player_get_y(player);
	map_set_cell_type(map, x, y, CELL_BOMB);
}

// Changes the state of the bomb
void bomb_change_state(struct bomb* bomb, enum bomb_state state) {
	assert(bomb);
	bomb->state = state;
}

void bomb_change_time(struct bomb* bomb, Uint32 time) {
	assert(bomb);
	bomb->time = time;
}

void bomb_explode(struct game * game, struct map* map, struct bomb* bomb) {
	assert(map);
	assert(bomb);

	struct monster * monster;

	int x = bomb_get_x(bomb);
	int y = bomb_get_y(bomb);
	int range = bomb_get_range(bomb);
	int k;

	int px = player_get_x(game_get_player(game));
	int py = player_get_y(game_get_player(game));

	if (px == x && py == y) {
		player_wound(game_get_player(game));
	}
	if (map_get_cell_type(map, x, y) == CELL_M_BOMB) {
		monster = monster_search(game_get_monster_list(game), x, y);
		monster_kill(monster, map);
	}

	map_set_cell_type(map, x, y, CELL_EMPTY);

	//West
	k = 1;
	while (k < range + 1 && map_is_inside(map, x - k, y)) {
		if (map_is_explodable(map, x - k, y))
			map_set_cell_type(map, x - k, y, CELL_EMPTY);
		if (px == x - k && py == y)
			player_wound(game_get_player(game));

		switch (map_get_cell_type(map, x - k, y)) {
		case CELL_MONSTER:
			monster = monster_search(game_get_monster_list(game), x - k, y);
			monster_kill(monster, map);
			break;
		case CELL_CASE:
			bonus_drop(game, map, x - k, y);
			break;
		case CELL_SCENERY:
			k += range;
			break;
		default:
			break;
		}
		k++;
	}

	//East
	k = 1;
	while (k < range + 1 && map_is_inside(map, x + k, y)) {
		if (map_is_explodable(map, x + k, y))
			map_set_cell_type(map, x + k, y, CELL_EMPTY);
		if (px == x + k && py == y)
			player_wound(game_get_player(game));

		switch (map_get_cell_type(map, x + k, y)) {
		case CELL_MONSTER:
			monster = monster_search(game_get_monster_list(game), x + k, y);
			monster_kill(monster, map);
			break;
		case CELL_CASE:
			bonus_drop(game, map, x + k, y);
			break;
		case CELL_SCENERY:
			k += range;
			break;
		default:
			break;
		}
		k++;
	}

	//North
	k = 1;
	while (k < range + 1 && map_is_inside(map, x, y - k)) {
		if (map_is_explodable(map, x, y - k))
			map_set_cell_type(map, x, y - k, CELL_EMPTY);
		if (px == x && py == y - k)
			player_wound(game_get_player(game));

		switch (map_get_cell_type(map, x, y - k)) {
		case CELL_MONSTER:
			monster = monster_search(game_get_monster_list(game), x, y - k);
			monster_kill(monster, map);
			break;
		case CELL_CASE:
			bonus_drop(game, map, x, y - k);
			break;
		case CELL_SCENERY:
			k += range;
			break;
		default:
			break;
		}

		k++;
	}

	//South
	k = 1;
	while (k < range + 1 && map_is_inside(map, x, y + k)) {
		if (map_is_explodable(map, x, y + k))
			map_set_cell_type(map, x, y + k, CELL_EMPTY);
		if (px == x && py == y + k)
			player_wound(game_get_player(game));

		switch (map_get_cell_type(map, x, y + k)) {
		case CELL_MONSTER:
			monster = monster_search(game_get_monster_list(game), x, y + k);
			monster_kill(monster, map);
			break;
		case CELL_CASE:
			bonus_drop(game, map, x, y + k);
			break;
		case CELL_SCENERY:
			k += range;
			break;
		default:
			break;
		}
		k++;
	}

}

void bomb_explode_disp(struct map* map, struct bomb* bomb) {
	assert(map);
	assert(bomb);

	int x = bomb_get_x(bomb);
	int y = bomb_get_y(bomb);
	int range = bomb_get_range(bomb);

	//PLays an explosion on tile (x,y)
	if (map_get_cell_type(map, x, y) == CELL_M_BOMB)
		map_set_cell_type(map, x, y, CELL_M_BOMB_EXPLODED);
	else
		map_set_cell_type(map, x, y, CELL_BOMB_EXPLODED);

	int k;

	//West
	k = 1;
	while (k < range + 1 && map_is_inside(map, x - k, y)) {
		if (map_is_explodable(map, x - k, y)) {
			map_set_cell_type(map, x - k, y, CELL_BOMB_EXPLODED);
		}
		if (map_get_cell_type(map, x - k, y) == CELL_SCENERY) {
			k += range;
		}
		k++;
	}

	//East
	k = 1;
	while (k < range + 1 && map_is_inside(map, x + k, y)) {
		if (map_is_explodable(map, x + k, y)) {
			map_set_cell_type(map, x + k, y, CELL_BOMB_EXPLODED);
		}
		if (map_get_cell_type(map, x + k, y) == CELL_SCENERY) {
			k += range;
		}
		k++;
	}

	//North
	k = 1;
	while (k < range + 1 && map_is_inside(map, x, y - k)) {
		if (map_is_explodable(map, x, y - k)) {
			map_set_cell_type(map, x, y - k, CELL_BOMB_EXPLODED);
		}
		if (map_get_cell_type(map, x, y - k) == CELL_SCENERY) {
			k += range;
		}
		k++;
	}

	//South
	k = 1;
	while (k < range + 1 && map_is_inside(map, x, y + k)) {
		if (map_is_explodable(map, x, y + k)) {
			map_set_cell_type(map, x, y + k, CELL_BOMB_EXPLODED);
		}
		if (map_get_cell_type(map, x, y + k) == CELL_SCENERY) {
			k += range;
		}
		k++;
	}
}

int bomb_get_timer(struct bomb* bomb) {
	assert(bomb);
	return bomb->time;
}

void bomb_disp(struct map* map, struct game * game, struct bomb* bomb) {
	assert(bomb);
	int x = bomb_get_x(bomb);
	int y = bomb_get_y(bomb);

	Uint32 timer = (SDL_GetTicks() - bomb->time);

	if (timer < 1000) {
		if (map_get_cell_type(map, x, y) == CELL_M_BOMB) {
			map_set_cell_type(map, x, y, CELL_M_BOMB_4);
		} else {
			map_set_cell_type(map, x, y, CELL_BOMB_4);
		}
	}
	if (timer > 1000 && timer < 2000) {
		bomb_change_state(bomb, state_3);
		if (map_get_cell_type(map, x, y) == CELL_M_BOMB) {
			map_set_cell_type(map, x, y, CELL_M_BOMB_3);
		} else {
			map_set_cell_type(map, x, y, CELL_BOMB_3);
		}
	}
	if (timer > 2000 && timer < 3000) {
		bomb_change_state(bomb, state_2);
		if (map_get_cell_type(map, x, y) == CELL_M_BOMB) {
			map_set_cell_type(map, x, y, CELL_M_BOMB_2);
		} else {
			map_set_cell_type(map, x, y, CELL_BOMB_3);
		}
	}
	if (timer > 3000 && timer < 4000) {
		bomb_change_state(bomb, state_1);
		if (map_get_cell_type(map, x, y) == CELL_M_BOMB) {
			map_set_cell_type(map, x, y, CELL_M_BOMB_1);
		} else {
			map_set_cell_type(map, x, y, CELL_BOMB_1);
		}
	}

	if (timer > 4000 && timer < 4300) {
		bomb_change_state(bomb, state_explosion);
		bomb_explode_disp(map, bomb);
	}

	if (timer > 4300 && timer < 4335) {
		bomb_explode(game, map, bomb);
	}

}

int bomb_get_x(struct bomb* bomb) {
	assert(bomb);
	return bomb->x;
}

int bomb_get_y(struct bomb* bomb) {
	assert(bomb);
	return bomb->y;
}

int bomb_get_range(struct bomb* bomb) {
	assert(bomb);
	return bomb->range;
}

