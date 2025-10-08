#ifndef CROCODILE_H
#define CROCODILE_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco



void draw_coccodrille(int starty, int startx, bool verso_flusso);

void delete_prev_croc(int x, int y);

void *crocodile_thread(void*arg);

void init_crocodile_args(C_Args **crocodile_args, Obj crocodiles[], int cocc_id, int speed, int direction,int cocc_x,int cocc_y,Mode game_mode);

void fill_crocodile(Obj crocodiles[NUM_COCCODRILLI], Obj bullet[NUM_COCCODRILLI]);

void crocodile_creation(Mode mode, C_Args *crocodile_args[NUM_COCCODRILLI],Obj crocodiles[NUM_COCCODRILLI],pthread_t thread_id_crocodiles[NUM_COCCODRILLI]);

void collision_bullet_to_croc(Obj* bullet, int id_bullet, Obj* crocodiles, bool* collision_bullet);

void crocodile_reset(Obj *crocodile, int *reset, int value);
#endif