#ifndef GRANADE_H
#define GRANADE_H


#include <sys/types.h>
#include "macro_struct.h"



void granades_process(int p_childs_to_father[2], int coordinata_y, int coordinata_x, int id);

void kill_grenades(int *id_granata, Obj frog, int p_childs_to_father[2], int *pid_grenades);

void fill_granades(Obj granate[MAX_GRANATE]);

void delete_before_granade(WINDOW *fixed_win, int x, int y);

void draw_granade_left(WINDOW *fixed_win, bool collision_left, int x, int y,char granade);

void draw_granade_right(WINDOW *fixed_win, bool collision_right, int x, int y,char granade);

void collision_check(WINDOW * fixed_win,Obj *granade, Obj crocodiles[NUM_COCCODRILLI], bool *collision_left, bool *collision_right);











#endif
