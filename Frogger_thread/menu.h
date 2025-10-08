#ifndef MENU_H
#define MENU_H

#include <sys/types.h> 
#include "macro_struct.h" // Strutture dati e macro usate nel gioco



Mode change_mode(bool easy, bool medium, bool hard);

Menu main_menu(WINDOW *win, int x, int y, bool flag, bool easy, bool medium, bool hard, int fine_partita[2]);

void draw_menu(WINDOW* win, int start_i, int start_j,int end_i, int end_j, char carattere, int color);

void draw_printw(WINDOW* win, int y, int x, char string[], int colore, int attr );

void draw_title(WINDOW *win);

void draw_boxes(WINDOW *win, int y, int x);



#endif