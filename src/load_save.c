/* load_save.c
 *
 * Created on: 4 mai 2014
 * Author: bwh
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include <game.h>
#include <load_save.h>
#include <constant.h>
#include <map.h>
#include <level.h>
#include <monster_list.h>
#include <bomb_list.h>

struct map * load_map(int map_num) {
	int x, y;
	int i, j;
	int a, b;
	int element;
	struct map * map;

	FILE* file;

	if (map_num == 1) {
		file = fopen("data/lvl_1_map_2.map", "r");
	}
	if (map_num == 2) {
		file = fopen("data/lvl_2_map_1.map", "r");
	}
	if (map_num == 3) {
		file = fopen("data/lvl_2_map_2.map", "r");
	}

	fscanf(file, "%d:%d\n", &i, &j);

	map = map_new(i, j);

	char * map1 = malloc(i * j);

	for (x = 0; x < i; x++) {
		for (y = 0; y < j; y++) {
			fscanf(file, "%d ", &element);
			map1[x * i + y] = (char) element;
		}
		fscanf(file, "\n");
	}

	char themap[i * j];

	for (x = 0; x < i * j; x++) {
		themap[x] = map1[x];
	}

	for (x = 0; x < i * j; x++) {
		a = x % i;
		b = ((x - a) / i);
		map_set_cell_type(map, a, b, themap[x]);
	}

	fclose(file);
	return map;
}

int save_game(struct game * game) {
	FILE * file = NULL;

	struct player * player = game_get_player(game);
	struct map * map = level_get_curr_map(game_get_curr_level(game));
	struct level * level = game_get_curr_level(game);
	int next_map = (level_get_map_nb(level) + 1) % 2;

	int w = map_get_width(map);
	int h = map_get_height(map);
	int i = 0;
	int j = 0;

	file = fopen("data/save.bwh", "w"); // Open write mode
	if (file == NULL) {
		printf("Error file not created");
		return 0;
	}

	//Saving player attributes
	fprintf(file, "%d ", player_get_nb_bomb(player));
	fprintf(file, "%d ", player_get_range(player));
	fprintf(file, "%d ", player_get_life(player));
	fprintf(file, "%d ", player_get_keys(player));
	fprintf(file, "%d ", player_is_god(player));
	fprintf(file, "%d ", game_has_C2(game));

	fprintf(file, "\n");

	// Saving level infos
	fprintf(file, "%d ", level_get_curr_nb(level));
	fprintf(file, "%d ", level_get_map_nb(level));

	fprintf(file, "\n");

	//Saving map infos
	fprintf(file, "%d:", w);
	fprintf(file, "%d ", h);

	fprintf(file, "\n");

	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++) {
			fprintf(file, "%d ", map_get_cell_complete_type(map, i, j));
		}
		fprintf(file, "\n");
	}

	//Saving the other map
	map = level_get_map(level, next_map);

	fprintf(file, "%d:", w);
	fprintf(file, "%d ", h);

	fprintf(file, "\n");

	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++) {
			fprintf(file, "%d ", map_get_cell_complete_type(map, i, j));
		}
		fprintf(file, "\n");
	}
	fclose(file);
	return 1;
}

void load_game(struct game * game) {
	FILE * file = NULL;

	int x, y, i, j, a, b;
	int element;
	int nb_bomb, range, life, keys, god, C2;
	int lvl_nb, map_nb;

	struct player * player;
	struct level * level;
	struct map * map;
	struct map * map2;

	file = fopen("data/save.bwh", "r"); // Open read mode

	// Retrieving player attributes
	fscanf(file, "%d ", &nb_bomb);
	fscanf(file, "%d ", &range);
	fscanf(file, "%d ", &life);
	fscanf(file, "%d ", &keys);
	fscanf(file, "%d ", &god);
	fscanf(file, "%d ", &C2);

	player = player_init(nb_bomb, range, life, keys);

	if (god == 1) {
		player_heal(player);
		player_wound(player); // Trick to activate god mode
	}

	fscanf(file, "\n");

	// Retrieving level attributes
	fscanf(file, "%d ", &lvl_nb);
	fscanf(file, "%d ", &map_nb);

	//
	// Retrieving the current map
	fscanf(file, "%d:%d\n", &i, &j);

	map = map_new(i, j);

	char * map1 = malloc(i * j);

	for (y = 0; y < i; y++) {
		for (x = 0; x < j; x++) {
			fscanf(file, "%d ", &element);
			map1[x * i + y] = (char) element;
		}
		fscanf(file, "\n");
	}

	char themap[i * j];

	for (x = 0; x < i * j; x++) {
		themap[x] = map1[x];
	}

	for (x = 0; x < i * j; x++) {
		a = x % i;
		b = ((x - a) / i);
		map_set_cell_type(map, a, b, themap[x]);
	}
	// Retrieve player position
	player_from_map(player, map);

	//
	// Retrieving the second map
	x = 0;
	y = 0;

	fscanf(file, "%d:%d\n", &i, &j);

	map2 = map_new(i, j);

	free(map1);
	map1 = malloc(i * j);

	for (y = 0; y < i; y++) {
		for (x = 0; x < j; x++) {
			fscanf(file, "%d ", &element);
			map1[x * i + y] = (char) element;
		}
		fscanf(file, "\n");
	}

	for (x = 0; x < i * j; x++) {
		themap[x] = map1[x];
	}

	for (x = 0; x < i * j; x++) {
		a = x % i;
		b = ((x - a) / i);
		map_set_cell_type(map2, a, b, themap[x]);
	}

	//Store in struct level
	level = level_build(lvl_nb, map_nb, map, map2);

	// Filling struct game with the collected informations
	game_build(game, level, player, bomb_list_init(), monster_list_init(), C2);
	monster_from_map(game_get_monster_list(game), map);

}
