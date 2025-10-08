#ifndef BULLET_H
#define BULLET_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco


void bullet_process(int p_childs_to_father[2], int coordinata_y, int coordinata_x, int id, bool direzione, int speed_bullet);

void kill_bullet(int p_childs_to_father[2], Obj crocodile, int *pid_bullet, bool *flag_bullet, time_t *next_bullet_time);


#endif