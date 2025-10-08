#ifndef FROG_H
#define FROG_H


#include <sys/types.h>
#include "macro_struct.h"



void draw_frog(WINDOW *fixed_win, int starty, int startx);

void delete_prev_frog(WINDOW *fixed_win, int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW], char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW], int x, int y);

void frog_process(int p_childs_to_father[2], int p_father_to_frog[2], WINDOW **fixed_win, Mode mode);

void frog_out_croc(bool *isFirstTimeOnCroc,Obj *obj, int *tempo,Obj crocodiles[NUM_COCCODRILLI]);

void check_frog_bullet_collision(Obj *frog, Obj bullet[], int num_bullets, int *tempo);

void check_frog_on_crocodile(Obj *obj, Obj crocodiles[NUM_COCCODRILLI], bool *frog_on_crocodile, int *crocodile_index, int *tempo);

void is_first_time_on_crocodile(Obj *obj, Obj *crocodiles, int *crocodile_index,bool *isFirstTimeOnCroc, int *id, int *posX, int *posY);

void frog_already_on_croc(Obj *obj , Obj crocodiles[NUM_COCCODRILLI], int crocodile_index, int *posX, int *posY);




#endif 
