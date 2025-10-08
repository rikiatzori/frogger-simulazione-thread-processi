#ifndef CONSUMER_H
#define CONSUMER_H

#include <sys/types.h>    // Per pid_t

#include "macro_struct.h" // Strutture dati e macro usate nel gioco
#include "frog.h"
#include "granade.h"


void* consumer_thread(void* arg);

void chance_current_time(time_t start_time, time_t *ultimo_aggiornamento, bool game_paused);

void buffer_manager(Obj *current_obj);

void reset_manche( Obj granade[MAX_GRANATE],Obj current_obj,bool collision_bullet_frog, bool drop_on_the_river ,bool *reset,Mode *mode);

void end_game(Obj current_obj);

void collision_bullet_granade(Obj granade[MAX_GRANATE],Obj bullets[NUM_COCCODRILLI], Obj current_obj, int bullet_id);

void mutex_semaphore_destroy();

void kill_threads(pthread_t thread_id_consumer, pthread_t thread_id_rana, pthread_t thread_id_crocodiles[], Mode *consumer_args, C_Args *crocodile_args[], F_Args *frog_args, int end_game[]);
#endif // CONSUMER_H
