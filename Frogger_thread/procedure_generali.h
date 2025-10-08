#ifndef PROCEDURE_GENERALI_H
#define PROCEDURE_GENERALI_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco


void graphical_interface(Mode mode);

void create_colors();

void game_music();

void stop_music();

void resume_music();

void end_music();

void drawing(int y, int x, char carattere,int color, bool a_bold);

void buffer_initialize();

void produce_position(Obj *obj);

#endif