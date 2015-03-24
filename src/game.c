#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>
#include "game.h"
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <bomb.h>
#include "level.h"
#include "player.h"
#include <bomb_list.h>
#include <monster_list.h>
#include <load_save.h>

struct game {
	struct level* curr_level; // current level
	struct player* player;
	struct bomb_list * bomb_list;
	struct monster_list * monster_list;
	Uint32 time;
	int C2;
};

int win = 0;
int quit_the_game = 0;

struct game* game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));

	cur_door.x = 13;
	cur_door.y = 13;
	cur_flag.x = 13;
	cur_flag.y = 13;

	game->curr_level = level_get_level(game, 0); // get maps of the first level

	game->player = player_init(3, 2, 2, 0);
	player_from_map(game->player, level_get_map(game->curr_level, 0)); // get x,y of the player on the first map

	//Bomb list initialization
	game->bomb_list = bomb_list_init();

	//Monsters initialization
	game->monster_list = monster_list_init();
	monster_from_map(game->monster_list, level_get_map(game->curr_level, 0));

	game->time = SDL_GetTicks();
	game->C2 = 2;

	return game;
}

void game_set_monster_list(struct game * game) {
	game->monster_list = monster_list_init();
}

void game_set_bomb_list(struct game * game) {
	game->bomb_list = bomb_list_init();
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	level_free(game->curr_level);
	monster_list_free(game->monster_list);
	bomb_list_free(game->bomb_list);
}

struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

struct bomb_list * game_get_bomb_list(struct game * game) {
	assert(game);
	return game->bomb_list;
}

struct monster_list * game_get_monster_list(struct game * game) {
	assert(game);
	return game->monster_list;
}

struct level* game_get_curr_level(struct game* game) {
	return game->curr_level;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = level_get_curr_map(game_get_curr_level(game));
	struct player * player = game_get_player(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = 0;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = SIZE_BLOC;
	if (player_get_life(player) >= 0)
		window_display_image(sprite_get_number(player_get_life(player)), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_bomb(player)), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_range(player)), x, y);

	x = 3 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_keys(player)), x, y);

	x = 4 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_flag(), x, y);

	x = 4 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(level_get_curr_nb(game->curr_level)),
			x, y);

}

void game_display(struct game* game) {
	assert(game);

	window_clear();

	game_banner_display(game);
	level_display(game_get_curr_level(game));
	player_display(game->player);
	monster_list_display(game->monster_list);

	struct map * map = level_get_curr_map(game_get_curr_level(game));

	bomb_list_disp(game->bomb_list, game, map);

	window_refresh();
}

void game_pause(struct game * game, Uint32 time) {

	SDL_Event event;
	int resume = 0;
	int choice = 0;
	Uint32 delay;

	while (!resume) {
		choice = choice % 4;
		window_display_image(sprite_get_pause(choice), 0, 0);
		window_refresh();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_p:
					resume++;
					break;
				case SDLK_UP:
					choice--;
					break;
				case SDLK_DOWN:
					choice++;
					break;
				case SDLK_SPACE:
					switch (choice) {
					case 0:
						resume++;
						break;
					case 1:
						save_game(game);
						resume++;
						break;
					case 2:
						load_game(game);
						resume++;
						break;
					case 3:
						quit_the_game = 1;
						resume++;
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}

		}
	}

	delay = SDL_GetTicks() - time;
	bomb_list_delay(game->bomb_list, delay);
}

void game_special_circle(struct game *game) {
	struct player* player = game_get_player(game);
	struct map* map = level_get_curr_map(game_get_curr_level(game));

	int x = player_get_x(player);
	int y = player_get_y(player);
	int range = player_get_range(player);

	// West bomb
	if (map_is_inside(map, x - 1, y)) {
		if (player_get_nb_bomb(player) > 0
				&& map_get_cell_type(map, x - 1, y) == CELL_EMPTY) {
			game->bomb_list = list_add(game->bomb_list,
					bomb_set(x - 1, y, range));
			player_dec_nb_bomb(player);
		}
	}
	// East bomb
	if (map_is_inside(map, x + 1, y)) {
		if (player_get_nb_bomb(player) > 0
				&& map_get_cell_type(map, x + 1, y) == CELL_EMPTY) {
			game->bomb_list = list_add(game->bomb_list,
					bomb_set(x + 1, y, range));
			player_dec_nb_bomb(player);
		}
	}
	// North bomb
	if (map_is_inside(map, x, y - 1)) {
		if (player_get_nb_bomb(player) > 0
				&& map_get_cell_type(map, x, y - 1) == CELL_EMPTY) {
			game->bomb_list = list_add(game->bomb_list,
					bomb_set(x, y - 1, range));
			player_dec_nb_bomb(player);
		}
	}
	// South bomb
	if (map_is_inside(map, x, y + 1)) {
		if (player_get_nb_bomb(player) > 0
				&& map_get_cell_type(map, x, y + 1) == CELL_EMPTY) {
			game->bomb_list = list_add(game->bomb_list,
					bomb_set(x, y + 1, range));
			player_dec_nb_bomb(player);
		}
	}
}

short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = level_get_curr_map(game_get_curr_level(game));

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: // keyboard : esc
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				if (player_get_nb_bomb(player) > 0) {
					game->bomb_list = list_add(game->bomb_list,
							bomb_set(player_get_x(player), player_get_y(player),
									player_get_range(player)));
					player_dec_nb_bomb(player);
				}
				break;
			case SDLK_p:
				game_pause(game, SDL_GetTicks());
				break;
				//special "jutsus" (combos)
			case SDLK_a: //Circular explosion, costs up to 4 bombs
				game_special_circle(game);
				break;
			case SDLK_z: //Throws a bomb 3 cells in front of player, called "C2 Dragon". 1 per level
				if (game->C2 > 0) {
					game_special_c2(game);
					game->C2--;
				}
				break;
			default:
				break;
			}

			break;
		}
	}
	return 0;
}

int game_update(struct game* game) {
	struct map* map = level_get_curr_map(game->curr_level);
	struct player* player = game_get_player(game);
	Uint32 timing = SDL_GetTicks() - game->time;
	Uint32 god_time = SDL_GetTicks() - player_get_god_time(player);

	if (input_keyboard(game) == 1 || quit_the_game == 1)
		return 1; // exit game
	if (player_get_life(player) < 0) //If we are under 0 lives, end the game
		return 1;

	if (timing > 500) {
		monster_list_move(game, game->monster_list, map);
		game->time = SDL_GetTicks();
	}
	if (god_time > 2000) {
		player_unset_god(player);
	}
	// Checks if map or level must be changed
	if (cur_door.x == player_get_x(player)
			&& cur_door.y == player_get_y(player)) {
		map_set_cell_type(map, last_pos.x, last_pos.y, CELL_PLAYER);
		level_next_map(game);
	}
	if (cur_flag.x == player_get_x(player)
			&& cur_flag.y == player_get_y(player)) {
		map_set_cell_type(map, last_pos.x, last_pos.y, CELL_PLAYER);
		level_next(game);
	}

	return 0;
}

void game_special_c2(struct game *game) {
	struct player* player = game_get_player(game);
	struct map* map = level_get_curr_map(game_get_curr_level(game));

	int x = player_get_x(player);
	int y = player_get_y(player);
	int range = player_get_range(player);

	switch (player_get_curr_way(player)) {
	case WEST:
		x -= 3;
		break;
	case EAST:
		x += 3;
		break;
	case NORTH:
		y -= 3;
		break;
	case SOUTH:
		y += 3;
		break;
	}
	if (map_is_inside(map, x, y)) {
		if (player_get_nb_bomb(player) > 0
				&& map_get_cell_type(map, x, y) == CELL_EMPTY) {
			game->bomb_list = list_add(game->bomb_list, bomb_set(x, y, range));
			player_dec_nb_bomb(player);
		}
	}
}

void game_build(struct game * game, struct level *level, struct player * player,
		struct bomb_list * bombs, struct monster_list * monsters, int C2) {
	game->C2 = C2;
	game->bomb_list = bombs;
	game->curr_level = level;
	game->monster_list = monsters;
	game->player = player;
	game->time = SDL_GetTicks();

}

void game_set_level(struct game * game, struct level * level) {
	game->curr_level = level;
}

int game_has_C2(struct game * game) {
	assert(game);
	return game->C2;
}

void start_screen() {
	SDL_Event event;
	int go = 0;

	while (!go) {
		window_display_image(sprite_get_pause(4), 0, 0);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN
					&& event.key.keysym.sym == SDLK_SPACE) {
				go++;
			}
		}
	}
}

void win_screen() {

	SDL_Event event;
	int quit = 0;

	while (!quit) {
		window_display_image(sprite_get_pause(5), 0, 0);
		window_refresh();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN
					&& event.key.keysym.sym == SDLK_SPACE) {
				quit++;
			}
		}
	}
	win = 1;
	quit_the_game = 1;
}

void lose_screen() {
	SDL_Event event;
	int quit = 0;

	while (!quit) {
		window_display_image(sprite_get_pause(6), 0, 0);
		window_refresh();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN
					&& event.key.keysym.sym == SDLK_SPACE) {
				quit++;
			}
		}
	}
	quit_the_game = 1;
}
