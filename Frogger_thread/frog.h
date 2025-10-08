#ifndef FROG_H
#define FROG_H


#include <sys/types.h>
#include "macro_struct.h"



void draw_frog(int starty, int startx);

void delete_prev_frog(int x, int y,bool in_tana,int centro_tana);

void* frog_thread(void* arg);

void init_frog_args(F_Args **frog_args,Obj * frog,Mode game_mode, int end_game[2]);

void fill_hole(int x_inizio[NUM_HOLE], int x_fine[NUM_HOLE]);

void manage_entrance_hole(Obj *obj,int x_inizio[NUM_HOLE], int x_fine[NUM_HOLE], bool tane_visitate[NUM_HOLE],bool*rana_in_tana);

void check_frog_bullet_collision(Obj *frog, Obj bullet[], bool * collision_bullet_frog);

void check_frog_on_crocodile(Obj *obj, Obj crocodiles[NUM_COCCODRILLI], bool *frog_on_crocodile, int *crocodile_index);

void is_first_time_on_crocodile(Obj *obj, Obj crocodiles[NUM_COCCODRILLI], int *crocodile_index,bool *isFirstTimeOnCroc, int *id, int *posX);

void frog_already_on_croc(Obj *obj , Obj crocodiles[NUM_COCCODRILLI], int crocodile_index, int *posX);

void frog_out_croc(bool *isFirstTimeOnCroc, Obj *obj,Obj crocodiles[NUM_COCCODRILLI],bool *drop_on_the_river);

void reset_frog(int* reset_triggered, Obj* frog, int x_inizio[], int x_fine[], bool tane_visitate[], bool* rana_in_tana, int* grenade_shoot_id);

#endif 
