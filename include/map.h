#ifndef MAP_H_
#define MAP_H_

typedef enum cell_type {
	CELL_EMPTY = 0,
	CELL_GOAL, // 1
	CELL_SCENERY, // 2
	CELL_PLAYER, // 3
	CELL_CASE, // 4
	CELL_BONUS, // 5
	CELL_MONSTER, // 6
	CELL_BOMB, // 7
	CELL_KEY, // 8
	CELL_DOOR, // 9
	CELL_CLOSED_DOOR, // 10
	CELL_FLAG, //11
	CELL_M_BOMB = 15 //15
} cell_type_t;

typedef enum bonus_type {
	BONUS_BOMB_RANGE_INC = 0,
	BONUS_BOMB_RANGE_DEC, // 1
	BONUS_BOMB_NB_INC, // 2
	BONUS_BOMB_NB_DEC, // 3
	BONUS_LIFE, // 4
	BONUS_MONSTER // 5
} bonus_type_t;

enum scenery_type {
	SCENERY_STONE, // 0
	SCENERY_TREE // 1
};

enum bomb_type {
	BOMB_EXPLODED, //0
	BOMB_1, //1
	BOMB_2, //2
	BOMB_3, //3
	BOMB_4 //4
};

enum compose_type {
	CELL_STONE = CELL_SCENERY | (SCENERY_STONE << 4), // 0010 0000
	CELL_TREE = CELL_SCENERY | (SCENERY_TREE << 4), // 0010 0001

	CELL_BONUS_BOMBINC = CELL_BONUS | (BONUS_BOMB_NB_INC << 4), // 0100 0010
	CELL_BONUS_BOMBDEC = CELL_BONUS | (BONUS_BOMB_NB_DEC << 4), // 0100 0011
	CELL_BONUS_RANGEINC = CELL_BONUS | (BONUS_BOMB_RANGE_INC << 4), // 0100 0000
	CELL_BONUS_RANGEDEC = CELL_BONUS | (BONUS_BOMB_RANGE_DEC << 4), // 0100 0001
	CELL_BONUS_LIFE = CELL_BONUS | (BONUS_LIFE << 4), // 0100 0100
	CELL_BONUS_MONSTER = CELL_BONUS | (BONUS_MONSTER << 4), // 0100 0101

	CELL_BOMB_EXPLODED = CELL_BOMB | (BOMB_EXPLODED << 4), // 0111 0000
	CELL_BOMB_1 = CELL_BOMB | (BOMB_1 << 4), // 0111 0001
	CELL_BOMB_2 = CELL_BOMB | (BOMB_2 << 4), // 0111 0010
	CELL_BOMB_3 = CELL_BOMB | (BOMB_3 << 4), // 0111 0011
	CELL_BOMB_4 = CELL_BOMB | (BOMB_4 << 4), // 0111 0100

	CELL_M_BOMB_EXPLODED = CELL_M_BOMB | (BOMB_EXPLODED << 4), // 1111 0000
	CELL_M_BOMB_1 = CELL_M_BOMB | (BOMB_1 << 4), // 1111 0001
	CELL_M_BOMB_2 = CELL_M_BOMB | (BOMB_2 << 4), // 1111 0010
	CELL_M_BOMB_3 = CELL_M_BOMB | (BOMB_3 << 4), // 1111 0011
	CELL_M_BOMB_4 = CELL_M_BOMB | (BOMB_4 << 4), // 1111 0100
};

struct map;

// Create a new empty map
struct map* map_new(int width, int height);
void map_free(struct map* map);

// Return the height and width of a map
int map_get_width(struct map* map);
int map_get_height(struct map* map);

// Return the type of a cell
cell_type_t map_get_cell_type(struct map* map, int x, int y);
enum compose_type map_get_cell_complete_type(struct map* map, int x, int y);
// Set the type of a cell
void map_set_cell_type(struct map* map, int x, int y, cell_type_t type);

// Test if (x,y) is within the map
int map_is_inside(struct map* map, int x, int y);

// Return a default 12x12 static map
struct map* map_get_default();

// Display the map on the screen
void map_display(struct map* map);
void display_bomb(struct map * map, int x, int y, char type);
void display_bonus(struct map* map, int x, int y, char type);
void display_scenery(struct map* map, int x, int y, char type);

// Checks if the cell could be exploded
int map_is_explodable(struct map* map, int x, int y);

#endif /* MAP_H_ */
