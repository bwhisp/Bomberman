/*
 * bomb_list.h
 *
 *  Created on: 22 avr. 2014
 *      Author: bwh
 */

#ifndef BOMB_LIST_H_
#define BOMB_LIST_H_

#include <bomb.h>

struct cell;
struct bomb_list;

//New list
struct bomb_list * bomb_list_init ();
//New element in the list
struct bomb_list * list_add (struct bomb_list * list, struct  bomb * bomb);
//Get infos on the list
struct  cell * list_get_first (struct bomb_list * list);
struct  bomb * list_get_bomb (struct  cell * cell );
struct  cell * get_next_cell (struct  cell * cell);

//Rides the list in order to display all the bombs
void bomb_list_disp(struct  bomb_list * list, struct game * game, struct  map * map);

// Modifies the bombs' timers so that they keep their states when game is paused
void bomb_list_delay (struct bomb_list * list, Uint32 delay);
// Frees the bomb list
void bomb_list_free (struct bomb_list * list);

#endif /* BOMB_LIST_H_ */
