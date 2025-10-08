#ifndef GRANADE_H
#define GRANADE_H


#include <sys/types.h>
#include "macro_struct.h"



void* granade_thread(void* arg);

void kill_grenades(int *id_granata, Obj frog, int p_childs_to_father[2], int *pid_grenades);

void fill_granades(Obj granate[MAX_GRANATE]);

void delete_before_granade( int x, int y);

void draw_granade_left(bool collision_left, int x, int y,char granade);

void draw_granade_right( bool collision_right, int x, int y,char granade);

void collision_check(Obj *granade, Obj crocodiles[NUM_COCCODRILLI], bool *collision_left, bool *collision_right);

void granade_args(int* grenade_shoot_id, Obj* frog, pthread_t thread_id_granade[]);









#endif
