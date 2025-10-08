#include "procedure_generali.h"
#include "macro_struct.h"


void graphical_interface(WINDOW **fixed_win,WINDOW **info,int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW],char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW],Mode mode){

    bkgd(COLOR_PAIR(0)); // imposto lo sfondo_colori per stdscr
    refresh(); // forzo la stampa
    
    // Creo la finestra di gioco fixed_win
    *fixed_win = newwin(HEIGHT_WINDOW, WIDTH_WINDOW, CENTER_Y, CENTER_X); // Creazione finestra principale
    wbkgd(*fixed_win, COLOR_PAIR(WHITE_BLACK)); // imposto lo sfondo_colori per fixed_win


    wattron(*fixed_win,COLOR_PAIR(RED_BLACK));
    //disegnamo i bordi verticali,spine
    for (int y = 0; y < HEIGHT_WINDOW ; y++) {
        mvwaddch(*fixed_win, y, 0, '>');  
        mvwaddch(*fixed_win, y, WIDTH_WINDOW - 1, '<');
    }
    wattroff(*fixed_win,COLOR_PAIR(RED_BLACK));


    keypad(*fixed_win , TRUE); // Abilito l'uso dei tasti speciali
    nodelay(*fixed_win , TRUE); // Rende l'input dalla finestra `fixed_win` non bloccante: le funzioni come `wgetch` non aspettano che l'utente prema un tasto.
    

    // Finestra info-------------------------------------------------------------//
    
    *info = newwin(HEIGHT_INFO, WIDTH_WINDOW, ORIGIN_Y, CENTER_X);

    wbkgd(*info, COLOR_PAIR(WHITE_BLACK));
    box(*info, 0, 0);
    nodelay(*info,TRUE);
    keypad(*info , TRUE); // Abilito l'uso dei tasti speciali
    
    // VALORI_INFO
    wattron(*info, COLOR_PAIR(RED_BLACK));
    mvwprintw(*info, DATI_INFO_Y, WIDTH_WINDOW-6, "TIME");
    mvwprintw(*info, DATI_INFO_Y, LIVES_INFO_X, "LIVES:%d",mode.vite_rana);
    mvwprintw(*info, DATI_INFO_Y, SCORE_INFO_X, "HI-SCORE:");
    wattroff(*info, COLOR_PAIR(RED_BLACK));

    wrefresh(*info);


    // PRATO VERDE------------------------------//
    //marciapiede di sopra
    wattron(*fixed_win, COLOR_PAIR(RED_GREEN));
    for (int i=1;i < END_FIUME; i++) {
        for (int j = 1; j < WIDTH_WINDOW-1; j++) {
            sfondo_carattere[i][j]=' ';
            sfondo_colori[i][j] = COLOR_PAIR(RED_GREEN);
            mvwaddch(*fixed_win, i, j, ' ');
        }
    }
    wattroff(*fixed_win, COLOR_PAIR(RED_GREEN));

    //marciapiede di sotto
    wattron(*fixed_win, COLOR_PAIR(RED_GREEN));
    for (int i=24;i < HEIGHT_WINDOW-1; i++) {
        for (int j = 1; j < WIDTH_WINDOW-1; j++) {
            sfondo_carattere[i][j]=' ';
            sfondo_colori[i][j] = COLOR_PAIR(RED_GREEN);
            mvwaddch(*fixed_win, i, j, ' ');
        }
    }
    wattroff(*fixed_win, COLOR_PAIR(RED_GREEN));


    // FIUME-----------------------------------//
    wattron(*fixed_win, COLOR_PAIR(BLUE_BLUE));
    for ( int i = END_FIUME; i < HEIGHT_WINDOW-5; i++) {
        for (int j = 1; j < WIDTH_WINDOW-1; j++) {
            sfondo_carattere[i][j]=' ';
            sfondo_colori[i][j] = COLOR_PAIR(BLUE_BLUE);
            mvwaddch(*fixed_win, i, j, ' ');                   
        }
    }
    wattroff(*fixed_win, COLOR_PAIR(BLUE_BLUE));


    // TANE-----------------------------------------------------//
    wattron(*fixed_win, COLOR_PAIR(BLUE_BLUE));
    for (int count = 0; count < NUM_HOLE; count++) {
        int x = HOLE_POSITION_X + count * (WIDTH_HOLE + ((WIDTH_WINDOW/10)-1));
        for (int i = 1; i <= 3; i++) { // Altezza dei blocchi
            for (int j = x; j < x + WIDTH_HOLE; j++) {
                sfondo_carattere[i][j]=' ';
                sfondo_colori[i][j] = COLOR_PAIR(BLUE_BLUE);
                mvwaddch(*fixed_win, i, j, ' ');
            }
        }
    }
    wattroff(*fixed_win, COLOR_PAIR(BLUE_BLUE));

}

void create_colors(){

    // modifica il colore nero
    init_color(COLOR_BLACK, 60, 60, 60); 

    // Inizializza il blu scuro
    init_color(COLOR_BLUE, 0, 0, 300);
    //inizializza il giallo
    init_color(COLOR_YELLOW, 1000, 600, 0);
    // Inizializza il verde scuro
    init_color(COLOR_GREEN, 0, 300, 0); 
    //inizializza l'arancione
    init_color(COLOR_ORANGE, 1000, 500, 0);  
    //inizializza il colore gold 
    init_color(COLOR_GOLD, 1000, 843, 0);
    
    // Creazione coppie colori, il primo colore è sempre il carattere, il secondo lo sfondo del carattere
    init_pair(GREEN_WHITE, COLOR_GREEN, COLOR_WHITE);
    init_pair(MAGENTA_BLACK, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
    init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK); // Colore finestra gioco
    init_pair(BLUE_BLUE, COLOR_BLUE, COLOR_BLUE);     // Colore fiume
    init_pair(RED_GREEN, COLOR_RED, COLOR_GREEN);     // Colore prato
    init_pair(RED_BLACK, COLOR_RED, COLOR_BLACK);     // valori info
    init_pair(YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK); //proiettili coccodrillo
    init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK); // Corpo del coccodrillo
    init_pair(RED_BLUE, COLOR_RED, COLOR_BLUE);     //granate rana
    init_pair(RED_MAGENTA, COLOR_RED, COLOR_MAGENTA);   //"bocca" rana
    init_pair(WHITE_MAGENTA, COLOR_WHITE, COLOR_MAGENTA);   //occhi rana

    //colori che mi servono per il menu
    init_pair(BLACK_MAGENTA, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(YELLOW_BLUE, COLOR_YELLOW, COLOR_BLUE);
    init_pair(BLACK_ORANGE, COLOR_BLACK, COLOR_ORANGE);
    init_pair(GOLD_RED, COLOR_GOLD ,COLOR_RED);

}

void game_music(){

    system("mpg123 -q --loop -1 sounds/songIntroAndMain.mp3 &");
}

void stop_music(){
    system("pkill -STOP mpg123");
}

void resume_music(){
    // Riprende la musica con SIGCONT
    system("pkill -CONT mpg123");
}

void end_music(){
    // Ferma definitivamente la musica
    system("pkill -KILL mpg123");

}

// Funzione che serve per disegnare un singolo carattere con l'alternativa di mettere il carattere in A_BOLD 
void drawing(WINDOW* fixed_win,int y, int x, char carattere,int color,bool a_bold){

    if(a_bold){
        
        wattron(fixed_win, COLOR_PAIR(color) | A_BOLD);
        mvwaddch(fixed_win, y, x, carattere); 
        wattroff(fixed_win, COLOR_PAIR(color) | A_BOLD);
    }else{
        wattron(fixed_win, COLOR_PAIR(color));
        mvwaddch(fixed_win, y, x, carattere);
        wattroff(fixed_win, COLOR_PAIR(color));
    }
}
