#ifndef FATHER_H
#define FATHER_H

#include <sys/types.h>    // Per pid_t

#include "macro_struct.h" // Strutture dati e macro usate nel gioco
#include "frog.h"
#include "granade.h"


void father_process(WINDOW **fixed_win, WINDOW **info, int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW], char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW], int p_childs_to_father[2], int p_father_to_frog[2], int p_father_to_croc[NUM_COCCODRILLI][2], int pid_frog, int pids_croc[NUM_COCCODRILLI], Obj frog, int fine_partita[2], Mode mode);

void initialize_pipes(int p_childs_to_father[2], int p_father_to_frog[2], int p_father_to_croc[NUM_COCCODRILLI][2]);

void fill_hole(int x_inizio[NUM_HOLE], int x_fine[NUM_HOLE], int num_tane);

void manage_entrance_hole(Obj *obj, WINDOW *fixed_win, WINDOW *info, int x_inizio[NUM_HOLE], int x_fine[NUM_HOLE], bool tane_visitate[NUM_HOLE], bool *rana_in_tana);

void chance_current_time(time_t start_time, time_t *ultimo_aggiornamento, WINDOW *info, bool game_paused);

void manage_pause(bool game_paused, Obj obj, Obj granate[MAX_GRANATE], Obj bullet[NUM_COCCODRILLI], Obj crocodiles[NUM_COCCODRILLI]);

void terminate_children(pid_t pid_frog, pid_t pids_croc[], int p_childs_to_father[2], int p_father_to_frog[2], int p_father_to_croc[][2]);

void frog_reset(WINDOW * info ,bool rana_in_tana, Obj *frog, int p_father_to_frog[2]);

void crocodile_reset(int p_father_to_croc[NUM_COCCODRILLI][2], Mode mode );



#endif // FATHER_H
