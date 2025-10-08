#ifndef BULLET_H
#define BULLET_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco


void *bullet_thread(void*arg);

void init_bullet_args(Obj *crocodile, Mode current_mode, pthread_t *thread_id_bullet,B_Args **bullet_args, bool *flag_bullet, time_t *next_bullet_time,time_t live_time);


#endif