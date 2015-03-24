#include <assert.h>
#include <stdlib.h>
#include <stddef.h> // NULL (macros)
#include <level.h>
#include <game.h>
#include <map.h>
#include <monster_list.h>
#include <monster.h>
#include <bomb_list.h>
#include <load_save.h>

struct level {
	struct map** maps; // array of the level's maps
	short nb_maps; // nb maps of the level
	short cur_map; // the current map
	short lvl_nb;
};

struct level* level_get_level(struct game * game, short num) {
	struct level* level = malloc(sizeof(*level));
	switch (num) {
	case 0:
		level->nb_maps = 2;
		level->cur_map = 0;
		level->lvl_nb = 0;
		level->maps = malloc(sizeof(*level->maps));
		level->maps[0] = map_get_default();
		level->maps[1] = load_map(1);
		return level;
		break;
	case 1:
		level->nb_maps = 2;
		level->cur_map = 0;
		level->lvl_nb = 1;
		level->maps = malloc(sizeof(*level->maps));
		level->maps[0] = load_map(2);
		level->maps[1] = load_map(3);
		return level;
		break;
	}

	return NULL;
}

struct map* level_get_curr_map(struct level* level) {
	return level->maps[level->cur_map];
}

short level_get_curr_nb(struct level * level) {
	return level->lvl_nb;
}

short level_get_maps(struct level * level) {
	return level->nb_maps;
}

struct map* level_get_map(struct level* level, int num) {
	assert(num <= level->nb_maps);
	return level->maps[num];
}

void level_free(struct level* level) {
	for (int i = 0; i < level->nb_maps; i++)
		map_free(level->maps[i]);
	free(level->maps);
}

void level_display(struct level* level) {
	map_display(level->maps[level->cur_map]);
}

struct map ** level_get_map_array(struct level * level) {
	return level->maps;
}

void level_set_map(struct level * level, short map_nb) {
	level->cur_map = map_nb;
}

short level_get_map_nb(struct level * level) {
	return level->cur_map;
}

void level_next_map(struct game * game) {
	struct level * level = game_get_curr_level(game);
	struct map * map = level_get_curr_map(level);
	short map_nb = level_get_map_nb(level);

	struct player * player = game_get_player(game);
	int x = player_get_x(player);
	int y = player_get_y(player);

	if (x == cur_door.x && y == cur_door.y) {
		//Recovers the door
		map_set_cell_type(map, x, y, CELL_DOOR);

		// New lists
		monster_list_free(game_get_monster_list(game));
		bomb_list_free(game_get_bomb_list(game));
		game_set_monster_list(game);
		game_set_bomb_list(game);

		//We have 2 maps per level
		map_nb = (map_nb + 1) % 2;

		level_set_map(level, map_nb);
		map = level_get_curr_map(level);

		//Update player position and fill the monster's list
		player_from_map(game_get_player(game), map);
		monster_from_map(game_get_monster_list(game), map);
	}
	// To avoid glitch
	cur_door.x = -1;
	cur_door.y = -1;
}

void level_next(struct game * game) {
	struct level * cur_level = game_get_curr_level(game);
	short num = (level_get_curr_nb(cur_level) + 1) % 2; // We only have 2 levels
	struct map * new_map;

	struct level * level = level_get_level(game, num);

	level_free(cur_level);

	// New lists
	monster_list_free(game_get_monster_list(game));
	bomb_list_free(game_get_bomb_list(game));
	game_set_monster_list(game);
	game_set_bomb_list(game);

	game_set_level(game, level);

	new_map = level_get_curr_map(level);

	//Update player and monster positions
	player_from_map(game_get_player(game), new_map);
	monster_from_map(game_get_monster_list(game), new_map);

	// To avoid glitch
	cur_flag.x = -1;
	cur_flag.y = -1;
}

struct level * level_build(short lvl_nb, short cur_map, struct map * map1,
		struct map * map2) {
	struct level * level = malloc(sizeof(*level));

	level->cur_map = cur_map;
	level->lvl_nb = lvl_nb;
	level->nb_maps = 2;

	level->maps = malloc(sizeof(*level->maps));
	level->maps[cur_map] = map1;
	cur_map = (cur_map + 1) % 2;
	level->maps[cur_map] = map2;

	return level;
}
