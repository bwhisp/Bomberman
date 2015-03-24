/*
 * monster_list.h
 *
 *  Created on: 8 mai 2014
 *      Author: bwh
 */

#ifndef MONSTER_LIST_H_
#define MONSTER_LIST_H_

#include <monster.h>
#include <game.h>

struct cell;
struct monster_list;

//New list
struct monster_list * monster_list_init ();
//New element in the list
struct monster_list * m_list_add (struct monster_list * list, struct  monster * monster);
//Get infos on the list
struct  cell * m_list_get_first (struct monster_list * list);
struct  monster * list_get_monster (struct  cell * cell );
struct  cell * m_get_next_cell (struct  cell * cell);

//Rides the list in order to display all the monsters
void monster_list_display (struct monster_list * list);

void monster_from_map(struct monster_list* list, struct map* map) ;

void monster_list_move(struct game * game, struct monster_list * list, struct map * map);

struct monster * monster_search(struct monster_list * list, int x, int y);
// Frees the monster list
void monster_list_free (struct monster_list * list);

#endif /* MONSTER_LIST_H_ */
