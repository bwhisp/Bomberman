#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <bomb.h>

struct map {
	int width;
	int height;
	char* grid;
};

#define CELL(i,j) (i +  map->width * j)

struct map* map_new(int width, int height) {
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof(*map));
	if (map == NULL)
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;

	map->grid = malloc(height * width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			map->grid[CELL(i,j)] = CELL_EMPTY;

	return map;
}

int map_is_inside(struct map* map, int x, int y) {
	assert(map);
	if (x < 0 || x >= map->width || y < 0 || y >= map->height)
		return 0;
	else
		return 1;
}

// Checks if the cell can explode
int map_is_explodable(struct map* map, int x, int y) {
	if (map_is_inside(map, x, y)) {
		switch (map_get_cell_type(map, x, y)) {
		case CELL_EMPTY:
			return 1;
			break;
		case CELL_CASE:
			return 0;
			break;
		case CELL_BOMB:
			return 1;
			break;
		case CELL_PLAYER:
			return 1;
			break;
		case CELL_BONUS:
			return 1;
			break;
		case CELL_GOAL:
			return 0;
		case CELL_KEY:
			return 0;
		default:
			break;
		}

	}
	return 0;
}

void map_free(struct map* map) {
	if (map == NULL)
		return;
	free(map->grid);
	free(map);
}

int map_get_width(struct map* map) {
	assert(map != NULL);
	return map->width;
}

int map_get_height(struct map* map) {
	assert(map);
	return map->height;
}

enum cell_type map_get_cell_type(struct map* map, int x, int y) {
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 15;
}

enum compose_type map_get_cell_complete_type(struct map* map, int x, int y) {
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)];
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type) {
	assert(map);
	assert(map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}

void display_bonus(struct map* map, int x, int y, char type) {
	// bonus is encoded with the 4 most significant bits
	switch (type >> 4) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
		break;
	case BONUS_LIFE:
		window_display_image(sprite_get_bonus(BONUS_LIFE), x, y);
		break;
	}
}

void display_bomb(struct map * map, int x, int y, char type) {
	switch (type >> 4) { // the state of the bomb is encoded with the 4 most significant bits
	case BOMB_4:
		window_display_image(sprite_get_bomb(4), x, y);
		break;
	case BOMB_3:
		window_display_image(sprite_get_bomb(3), x, y);
		break;
	case BOMB_2:
		window_display_image(sprite_get_bomb(2), x, y);
		break;
	case BOMB_1:
		window_display_image(sprite_get_bomb(1), x, y);
		break;
	case BOMB_EXPLODED:
		window_display_image(sprite_get_bomb(0), x, y);
		break;
	}
}

void display_scenery(struct map* map, int x, int y, char type) {
	switch (type >> 4) { // sub-types are encoded with the 4 most significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	}
}

void map_display(struct map* map) {
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++) {
		for (int j = 0; j < map->height; j++) {
			x = i * SIZE_BLOC;
			y = j * SIZE_BLOC;

			char type = map->grid[CELL(i,j)];

			switch (type & 15) { // type is encoded with 4 bits, 15 (1111) is a mask to keep the four less significant bits
			case CELL_SCENERY:
				display_scenery(map, x, y, type);
				break;
			case CELL_CASE:
				window_display_image(sprite_get_box(), x, y);
				break;
			case CELL_BONUS:
				display_bonus(map, x, y, type);
				break;
			case CELL_KEY:
				window_display_image(sprite_get_key(), x, y);
				break;
			case CELL_DOOR:
				window_display_image(sprite_get_door(), x, y);
				break;
			case CELL_CLOSED_DOOR:
				window_display_image(sprite_get_closed_door(), x, y);
				break;
			case CELL_BOMB:
				display_bomb(map, x, y, type);
				break;
			case CELL_FLAG:
				window_display_image(sprite_get_flag(), x, y);
				break;
			case CELL_GOAL:
				window_display_image(sprite_get_princess(), x, y);
				break;
			}
		}

	}
}

struct map* map_get_default(void) {
	struct map* map = map_new(MAP_WIDTH, MAP_HEIGHT);

	char themap[MAP_WIDTH * MAP_HEIGHT] = { CELL_PLAYER, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,
			CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY,
			CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_STONE, CELL_CASE, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
			CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_CASE, CELL_STONE,
			CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,
			CELL_CASE, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY,
			CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_MONSTER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_TREE, CELL_CASE, CELL_TREE, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_TREE, CELL_TREE, CELL_TREE,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
			CELL_CASE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,
			CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,
			CELL_CASE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
			CELL_EMPTY, CELL_KEY, CELL_EMPTY, CELL_CLOSED_DOOR };

	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
		map->grid[i] = themap[i];

	return map;
}

