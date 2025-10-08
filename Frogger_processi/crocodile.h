#ifndef CROCODILE_H
#define CROCODILE_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco



void draw_coccodrille(WINDOW *fixed_win, int starty, int startx, bool verso_flusso);

void delete_prev_croc(WINDOW *fixed_win, int x, int y);

void crocodile_process(int p_childs_to_father[2], int p_father_to_croc[NUM_COCCODRILLI][2], int cocc_x, int cocc_y, bool direction, int cocc_id, int speed, Mode mode);

void fill_crocodile(Obj crocodiles[NUM_COCCODRILLI], Obj bullet[NUM_COCCODRILLI]);

void crocodile_creation(int p_childs_to_father[2], int p_father_to_croc[NUM_COCCODRILLI][2], int pids_croc[NUM_COCCODRILLI],  Mode mode);

void collision_bullet_to_croc(Obj* bullet, int id_bullet, Obj* crocodiles, bool* collision_bullet, WINDOW** fixed_win);


#endif