#ifndef PROCEDURE_GENERALI_H
#define PROCEDURE_GENERALI_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco


void graphical_interface(WINDOW **fixed_win, WINDOW **info, int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW], char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW], Mode mode);

void create_colors();

void game_music();

void stop_music();

void resume_music();

void end_music();

void drawing(WINDOW* fixed_win,int y, int x, char carattere,int color, bool a_bold);


#endif