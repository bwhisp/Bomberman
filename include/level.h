#ifndef LEVEL_H_
#define LEVEL_H_

#include <map.h>
#include <game.h>

struct level;
struct game;

struct door {
	int x, y;
};

struct flag {
	int x, y;
};

struct flag cur_flag;
struct door cur_door;

// return level number num
struct level* level_get_level(struct game * game, short num);
// return current map of the level
struct map* level_get_curr_map(struct level* level);
//returns the current level number
short level_get_curr_nb(struct level * level);
//returns the amount of maps of the current level
short level_get_maps(struct level * level);
//returns the map number num of the current level
struct map* level_get_map(struct level* level, int num);
// return number of the current map
short level_get_curr_nb(struct level * level);
// return map number num of the level
struct map* level_get_map(struct level* level, int num);
// free the level
void level_free(struct level* level);
// display the level
void level_display(struct level* level);
//Gets the map array of the level
struct map ** level_get_map_array(struct level * level);
// Sets the map to map[map_nb] in the current level
void level_set_map(struct level * level, short map_nb);
// Returns the current map number
short level_get_map_nb(struct level * level);

// Gets next map/level
void level_next_map(struct game * game);
void level_next(struct game * game);

// Builds a level with the given parameters
struct level * level_build(short lvl_nb, short cur_map, struct map * map1,
		struct map * map2);

#endif /* LEVEL_H_ */
