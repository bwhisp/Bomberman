#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <bomb.h>
#include "bonus.h"
#include <load_save.h>


struct player {
	int x, y;
	enum way current_way;
	int nb_bomb;
	int bomb_range;
	int life;
	int keys;
	int god_mode;
	Uint32 god_time;
};

//Init & free
struct player* player_init(int bomb_number, int range, int life, int keys) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_way = SOUTH;
	player->nb_bomb = bomb_number;
	player->bomb_range = range;
	player->life = life;
	player->keys = keys;
	player->god_mode = 0;

	return player;
}

void player_free(struct player* player) {
	assert(player);
	free(player);
}

// Get infos
int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

int player_get_range(struct player* player) {
	assert(player != NULL);
	return player->bomb_range;
}

int player_get_life(struct player* player) {
	assert(player != NULL);
	return player->life;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bomb;
}

int player_get_keys(struct player* player) {
	assert(player);
	return player->keys;
}

int player_is_god(struct player * player) {
	assert(player);
	return player->god_mode;
}

Uint32 player_get_god_time(struct player * player) {
	assert(player);
	return player->god_time;
}

enum way player_get_curr_way(struct player * player) {
	assert(player);
	return player->current_way;
}
// Modifications
void player_set_current_way(struct player* player, enum way way) {
	assert(player);
	player->current_way = way;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	if (player->nb_bomb < 9)
		player->nb_bomb += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	if (player->nb_bomb > 0)
		player->nb_bomb -= 1;
}

void player_inc_range(struct player* player) {
	assert(player);
	if (player->bomb_range < 9)
		player->bomb_range += 1;
}

void player_dec_range(struct player* player) {
	assert(player);
	if (player->bomb_range > 1)
		player->bomb_range -= 1;
}

void player_wound(struct player * player) {
	assert(player);
	if (player->god_mode == 0 && !player_is_god(player)) {
		player->life--;
		player->god_time = SDL_GetTicks();
		player->god_mode = 1;
	}
}

void player_heal(struct player * player) {
	assert(player);
	if (player->life < 9)
		player->life++;
}

void player_inc_key(struct player * player) {
	assert(player);
	player->keys++;
}

void player_dec_key(struct player * player) {
	assert(player);
	player->keys--;
}

void player_unset_god(struct player * player) {
	assert(player);
	player->god_mode = 0;
}

// Map
void player_from_map(struct player* player, struct map* map) {
	assert(player);
	assert(map);

	int i, j;
	for (i = 0; i < map_get_width(map); i++) {
		for (j = 0; j < map_get_height(map); j++) {
			if (map_get_cell_type(map, i, j) == CELL_PLAYER) {
				player->x = i;
				player->y = j;
			}
		}
	}
}

static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_CASE:
		return 2;
		break;

	case CELL_BONUS:
		return 1;
		break;

	case CELL_GOAL:
		win_screen();
		return 1;
		break;

	case CELL_MONSTER:
		if (!player_is_god(player))
			player_wound(player);
		return 1;
		break;

	case CELL_BOMB:
		return 1;
		break;

	case CELL_KEY:
		return 1;
		break;

	case CELL_CLOSED_DOOR:
		if (player_get_keys(player) > 0) {
			map_set_cell_type(map, x, y, CELL_DOOR);
			cur_door.x = x;
			cur_door.y = y;
			return 1;
		}
		return 0;
		break;

	case CELL_DOOR:
		cur_door.x = x;
		cur_door.y = y;
		return 1;
		break;

	case CELL_PLAYER:
		break;

	case CELL_FLAG:
		cur_flag.x = x;
		cur_flag.y = y;
		return 1;
		break;
	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	last_pos.x = x;
	last_pos.y = y;

	switch (player->current_way) {
	case NORTH:
		if (player_move_aux(player, map, x, y - 1) == 1) {
			if (map_get_cell_type(map, x, y - 1) == CELL_KEY)
				player_inc_key(player);
			if (map_get_cell_type(map, x, y - 1) == CELL_DOOR
					&& player_get_keys(player) > 0)
				player_dec_key(player);
			if (map_get_cell_type(map, x, y - 1) == CELL_BONUS)
				bonus_get(map, player, x, y - 1);
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:
		if (player_move_aux(player, map, x, y + 1) == 1) {
			if (map_get_cell_type(map, x, y + 1) == CELL_KEY)
				player_inc_key(player);
			if (map_get_cell_type(map, x, y + 1) == CELL_DOOR
					&& player_get_keys(player) > 0)
				player_dec_key(player);
			if (map_get_cell_type(map, x, y + 1) == CELL_BONUS)
				bonus_get(map, player, x, y + 1);
			player->y++;
			move = 1;
		}
		break;

	case WEST:
		if (player_move_aux(player, map, x - 1, y) == 1) {
			if (map_get_cell_type(map, x - 1, y) == CELL_KEY)
				player_inc_key(player);
			if (map_get_cell_type(map, x - 1, y) == CELL_DOOR
					&& player_get_keys(player) > 0)
				player_dec_key(player);
			if (map_get_cell_type(map, x - 1, y) == CELL_BONUS)
				bonus_get(map, player, x - 1, y);
			player->x--;
			move = 1;
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y) == 1) {
			if (map_get_cell_type(map, x + 1, y) == CELL_KEY)
				player_inc_key(player);
			if (map_get_cell_type(map, x + 1, y) == CELL_DOOR
					&& player_get_keys(player) > 0)
				player_dec_key(player);
			if (map_get_cell_type(map, x + 1, y) == CELL_BONUS)
				bonus_get(map, player, x + 1, y);
			player->x++;
			move = 1;
		}
		break;
	}

	switch (player->current_way) {
	case NORTH:
		if (player_move_aux(player, map, x, y - 1) == 2
				&& player_move_aux(player, map, x, y - 2) == 1) {
			player->y--;
			map_set_cell_type(map, player->x, player->y - 1, CELL_CASE);
			move = 1;
		}
		break;
	case SOUTH:
		if (player_move_aux(player, map, x, y + 1) == 2
				&& player_move_aux(player, map, x, y + 2) == 1) {
			player->y++;
			map_set_cell_type(map, player->x, player->y + 1, CELL_CASE);
			move = 1;
		}
		break;
	case WEST:
		if (player_move_aux(player, map, x - 1, y) == 2
				&& player_move_aux(player, map, x - 2, y) == 1) {
			player->x--;
			map_set_cell_type(map, player->x - 1, player->y, CELL_CASE);
			move = 1;
		}
		break;
	case EAST:
		if (player_move_aux(player, map, x + 1, y) == 2
				&& player_move_aux(player, map, x + 2, y) == 1) {
			player->x++;
			map_set_cell_type(map, player->x + 1, player->y, CELL_CASE);
			move = 1;
		}
		break;
	}

	if (move) {
		map_set_cell_type(map, x, y, CELL_EMPTY);
		map_set_cell_type(map, player->x, player->y, CELL_PLAYER);
	}

	return move;
}

// Display
void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->current_way),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}

