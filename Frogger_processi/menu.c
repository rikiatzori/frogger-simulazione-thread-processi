#include "menu.h"
#include "macro_struct.h"
#include "procedure_generali.h"


//funzione che serve per colorare i menu principali di inizio e fine partita
void draw_menu(WINDOW* win, int start_i, int start_j,int end_i, int end_j, char carattere, int color){

    wattron(win, COLOR_PAIR(color));
        for (int i = start_i; i <= end_i; i++) {
            for (int j = start_j; j < end_j; j++) {
                mvwaddch(win, i, j, carattere);
            }
        }
    wattroff(win, COLOR_PAIR(color));

}

// Funzione che serve per scrivere nei menu principali
void draw_printw(WINDOW* win, int y, int x, char string[], int colore, int attr ){

    wattron(win, COLOR_PAIR(colore) | attr);
    mvwprintw(win, y, x, "%s", string); 
    wattroff(win, COLOR_PAIR(colore) | attr);

}

// Questa funzine mi serve per disegnare tutti i box che si trovano all'interno dei menu
void draw_boxes(WINDOW *win, int y, int x) {

    // Posizione e dimensioni del primo box
    int box1_y = POS_Y_BOX;   // Distanza di 3 caratteri dal bordo superiore
    int box1_x = POS_X_BOX;   // Distanza di 4 caratteri dal bordo sinistro
    int box1_h = HEIGHT_BOX;  // Altezza del primo box
    int box1_w = WIDTH_WINDOW - 8;  // Larghezza del primo box, lasciando margine ai lati


    // Disegna i bordi del primo box
    mvwaddch(win, box1_y, box1_x, ACS_ULCORNER); // Angolo in alto a sinistra
    mvwaddch(win, box1_y, box1_x + box1_w - 1, ACS_URCORNER); // Angolo in alto a destra
    mvwaddch(win, box1_y + box1_h - 1, box1_x, ACS_LLCORNER); // Angolo in basso a sinistra
    mvwaddch(win, box1_y + box1_h - 1, box1_x + box1_w - 1, ACS_LRCORNER); // Angolo in basso a destra


    // Disegna i bordi laterali del primo box
    mvwhline(win, box1_y, box1_x + 1, ACS_HLINE, box1_w - 2); // Linea orizzontale superiore
    mvwhline(win, box1_y + box1_h - 1, box1_x + 1, ACS_HLINE, box1_w - 2); // Linea orizzontale inferiore
    mvwvline(win, box1_y + 1, box1_x, ACS_VLINE, box1_h - 2); // Linea verticale sinistra
    mvwvline(win, box1_y + 1, box1_x + box1_w - 1, ACS_VLINE, box1_h - 2); // Linea verticale destra


    // Posizione e dimensioni del secondo box
    int space_between_boxes = 1; // Spazio di 1 carattere tra i due box
    int box2_y = box1_y + box1_h + space_between_boxes; // Posiziona il secondo box sotto il primo
    int box2_x = box1_x;  // Stessa x del primo box
    int box2_h = HEIGHT_BOX;  // Stessa altezza del primo box
    int box2_w = box1_w;  // Stessa larghezza del primo box


    // Disegna i bordi del secondo box
    mvwaddch(win, box2_y, box2_x, ACS_ULCORNER); // Angolo in alto a sinistra
    mvwaddch(win, box2_y, box2_x + box2_w - 1, ACS_URCORNER); // Angolo in alto a destra
    mvwaddch(win, box2_y + box2_h - 1, box2_x, ACS_LLCORNER); // Angolo in basso a sinistra
    mvwaddch(win, box2_y + box2_h - 1, box2_x + box2_w - 1, ACS_LRCORNER); // Angolo in basso a destra


    // Disegna i bordi laterali del secondo box
    mvwhline(win, box2_y, box2_x + 1, ACS_HLINE, box2_w - 2); // Linea orizzontale superiore
    mvwhline(win, box2_y + box2_h - 1, box2_x + 1, ACS_HLINE, box2_w - 2); // Linea orizzontale inferiore
    mvwvline(win, box2_y + 1, box2_x, ACS_VLINE, box2_h - 2); // Linea verticale sinistra
    mvwvline(win, box2_y + 1, box2_x + box2_w - 1, ACS_VLINE, box2_h - 2); // Linea verticale destra


    // Posizione e dimensioni dei tre nuovi box
    int small_box_w = box1_w / NUM_SMALL_BOX ;
    int small_box_h = HEIGHT_SMALL_BOX;
    int small_box_y = box2_y + box2_h + NUM_SMALL_BOX;
    int small_box_x1 = box1_x;
    int small_box_x2 = small_box_x1 + small_box_w + 1;
    int small_box_x3 = small_box_x2 + small_box_w + 1;


    // Disegna i tre box piccoli
    int box_x_positions[] = {small_box_x1, small_box_x2, small_box_x3};
    char *box_labels[] = {"E A S Y", " M E D I U M ", " H A R D "};
    int center_x;
    int center_y;

    
  // Ciclo per disegnare piu' riquadri (small boxes) 
    for (int i = 0; i < NUM_SMALL_BOX; i++) {

        // Ottiene la posizione X della small box corrente
        int x = box_x_positions[i];
        center_x= x + (small_box_w - strlen(box_labels[i])) / 2;
        center_y= small_box_y + small_box_h / 2;

        // Disegna gli angoli del riquadro
        mvwaddch(win, small_box_y, x, ACS_ULCORNER); // Angolo in alto a sinistra
        mvwaddch(win, small_box_y, x + small_box_w - 1, ACS_URCORNER); // Angolo in alto a destra
        mvwaddch(win, small_box_y + small_box_h - 1, x, ACS_LLCORNER); // Angolo in basso a sinistra
        mvwaddch(win, small_box_y + small_box_h - 1, x + small_box_w - 1, ACS_LRCORNER); // Angolo in basso a destra

        // Disegna le linee orizzontali superiore e inferiore
        mvwhline(win, small_box_y, x + 1, ACS_HLINE, small_box_w - 2); // Bordo superiore
        mvwhline(win, small_box_y + small_box_h - 1, x + 1, ACS_HLINE, small_box_w - 2); // Bordo inferiore

        // Disegna le linee verticali laterali
        mvwvline(win, small_box_y + 1, x, ACS_VLINE, small_box_h - 2); // Bordo sinistro
        mvwvline(win, small_box_y + 1, x + small_box_w - 1, ACS_VLINE, small_box_h - 2); // Bordo destro

        
        wattron(win, COLOR_PAIR(GREEN_BLACK) | A_BOLD);

        // Stampa l'etichetta della small box al centro
        mvwprintw(win,center_y, center_x, "%s", box_labels[i]);

        wattroff(win, COLOR_PAIR(GREEN_BLACK) | A_BOLD);

    }


    
    // Evidenziazione delle aree selezionate

    // se mi trovo nel primo box lo evidenzio
    if(y == COORDINATA_Y_BOX_1) { 
        
        draw_menu(win,box1_y +1,box1_x+1,HEIGHT_BOX+1,WIDTH_BOX + 3,' ',GREEN_WHITE);

        //argomento del primo box del menu
        draw_printw(win,COORDINATA_Y_BOX_1,COORDINATA_X_STRINGA_BOX1+5, " N E W  G A M E",GREEN_WHITE, A_BOLD);

    } else {    
        //Se mi sposto cancello l evidenziatura    
        draw_menu(win,box1_y+1, box1_x+1 ,HEIGHT_BOX+1, WIDTH_BOX +3,' ',RED_BLACK);
        
        //argomento del primo box del menu
        draw_printw(win,COORDINATA_Y_BOX_1,COORDINATA_X_STRINGA_BOX1+5, " N E W  G A M E",GREEN_BLACK, A_BOLD);

    }


    // se mi trovo nel secondo  box lo evidenzio
    if(y == COORDINATA_Y_BOX_2) {
        
        draw_menu(win,box2_y+1,box2_x+1, COORDINATA_Y_BOX_2+1, WIDTH_BOX+3, ' ', GREEN_WHITE);
        
        //argomento del secondo box del menu
        draw_printw(win,COORDINATA_Y_BOX_2 -1, COORDINATA_X_STRINGA_BOX2, "E X I T", GREEN_WHITE, A_BOLD);
     
    }else{
        
        //Se mi sposto cancello l evidenziatura 
        draw_menu(win,box2_y+1, box2_x+1 ,COORDINATA_Y_BOX_2+1, WIDTH_BOX +3,' ',RED_BLACK);

        //argomento del secondo box del menu
        draw_printw(win,COORDINATA_Y_BOX_2 -1, COORDINATA_X_STRINGA_BOX2,"E X I T",GREEN_BLACK,A_BOLD);

    }


    //BLOCCO EASY
    if(y >= COORDINATA_Y_SMALL_BOX && x == COORDINATA_X_EASY){   
        
        //se mi sposto nel blocco EASY lo evidenzio
        draw_menu(win,COORDINATA_Y_SMALL_BOX, START_EASY, MAX_Y_SMALL_BOX-1, END_EASY, ' ', YELLOW_BLUE);
        //argomento blocco EASY
        draw_printw(win,COORDINATA_Y_SMALL_BOX+1, COORDINATA_X_STRINGA_EASY,"E A S Y",YELLOW_BLUE,A_BOLD);

    }else{

        //Se mi sposto cancello l evidenziatura 
        draw_menu(win,COORDINATA_Y_SMALL_BOX,START_EASY,MAX_Y_SMALL_BOX-1,END_EASY,' ',GREEN_BLACK);
        draw_printw(win,COORDINATA_Y_SMALL_BOX+1, COORDINATA_X_STRINGA_EASY,"E A S Y",GREEN_BLACK,A_BOLD);

    }


    //BLOCCO MEDIUM
    if(y >= COORDINATA_Y_SMALL_BOX && x == COORDINATA_X_MEDIUM){    
       
        //se mi sposto nel blocco MEDIUM lo evidenzio
        draw_menu(win,COORDINATA_Y_SMALL_BOX, START_MEDIUM, MAX_Y_SMALL_BOX-1, END_MEDIUM, ' ', BLACK_ORANGE);
        //argomento blocco MEDIUM
        draw_printw(win,COORDINATA_Y_SMALL_BOX+1, COORDINATA_X_STRINGA_MEDIUM,"M E D I U M", BLACK_ORANGE, A_BOLD );

    }else{

        //Se mi sposto cancello l evidenziatura 
        draw_menu(win,COORDINATA_Y_SMALL_BOX, START_MEDIUM, MAX_Y_SMALL_BOX-1,END_MEDIUM, ' ',GREEN_BLACK);
        draw_printw(win,COORDINATA_Y_SMALL_BOX+1, COORDINATA_X_STRINGA_MEDIUM, "M E D I U M", GREEN_BLACK, A_BOLD);

    }

    
    //BLOCCO HARD
    if(y >= COORDINATA_Y_SMALL_BOX && x == COORDINATA_X_HARD){
               
        //se mi sposto nel blocco HARD lo evidenzio
        draw_menu(win,COORDINATA_Y_SMALL_BOX,START_HARD, MAX_Y_SMALL_BOX-1, END_HARD, ' ', GOLD_RED);
        //argomento blocco HARD
        draw_printw(win, COORDINATA_Y_SMALL_BOX+1, COORDINATA_X_STRINGA_HARD, "H A R D", GOLD_RED,A_BOLD);

    }else{

        //Se mi sposto cancello l evidenziatura 
        draw_menu(win,COORDINATA_Y_SMALL_BOX, START_HARD, MAX_Y_SMALL_BOX-1, END_HARD, ' ', GREEN_BLACK);
        draw_printw(win, COORDINATA_Y_SMALL_BOX + 1, COORDINATA_X_STRINGA_HARD, "H A R D", GREEN_BLACK, A_BOLD);

    }


    // Disegna un bordo intorno all'intera finestra
    wattron(win, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);
    box(win, 0, 0);
    wattroff(win, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);


    // Aggiorna la finestra per mostrare i box
    wrefresh(win);
}

void draw_title(WINDOW *win){

    mvwaddch(win, 1, 24, 'F');

    wattron(win, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);
    mvwaddch(win, 2, 26, 'R');
    mvwaddch(win, 1, 28, 'O');
    wattroff(win, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);

    mvwaddch(win, 2, 30, 'G');
    mvwaddch(win, 1, 32, 'G');

    wattron(win, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);
    mvwaddch(win, 2, 34, 'E');
    mvwaddch(win, 1, 36, 'R');
    wattroff(win, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);

}

// Funzione che serve per settare le modalità di gioco, attribuendo per ogni modalità dei valori specifici
Mode change_mode(bool easy, bool medium, bool hard){

    Mode mode;

    if (easy == TRUE){

        mode.vite_rana = 5;
        mode.speed_crocodile_min = SPEED_CROC_MIN_ES;
        mode.speed_crocodile_max = SPEED_CROC_MAX_ES;
        mode.speed_bullet = SPEED_BULLET_ES;
        mode.t_period_spawn_bullet = MAX_ES;

    }else if (medium == TRUE){

        mode.vite_rana = 3;
        mode.speed_crocodile_min = SPEED_CROC_MIN_MD;
        mode.speed_crocodile_max = SPEED_CROC_MAX_MD;
        mode.speed_bullet = SPEED_BULLET_MD;
        mode.t_period_spawn_bullet = MAX_MD;

    }else if (hard == TRUE){

        mode.vite_rana = 2;
        mode.speed_crocodile_min = SPEED_CROC_MIN_HD;
        mode.speed_crocodile_max = SPEED_CROC_MAX_HD;
        mode.speed_bullet = SPEED_BULLET_HD;
        mode.t_period_spawn_bullet = MAX_HD;

    }

    return mode;
}

Menu main_menu(WINDOW *game_menu, int x, int y, bool flag, bool easy, bool medium, bool hard, int fine_partita[2]){

    Menu menu;

    while (flag == TRUE){

        draw_boxes(game_menu, y, x);

        // LA PARTITA E TERMINATA
        if (fine_partita[0] != ERR){

            wattron(game_menu, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);

            // indico se il giocatore ha perso o vinto
            if (fine_partita[0] == NUM_HOLE){

                mvwprintw(game_menu, POS_Y_RISULTATO_PARTITA, POS_X_RISULTATO_PARTITA, "Y O U  W I N!!!");
            }else{

                mvwprintw(game_menu, POS_Y_RISULTATO_PARTITA, POS_X_RISULTATO_PARTITA, "G A M E  O V E R!!!");
            }

            mvwprintw(game_menu, POS_Y_RISULTATO_PARTITA, POS_X_SCORE_PARTITA, "S C O R E : %d ", fine_partita[1]);

            wattroff(game_menu, COLOR_PAIR(MAGENTA_BLACK) | A_BOLD);
        }

        draw_title(game_menu);
        
        draw_printw(game_menu,INFOSTARTY,INFOSTARTX,"Press < > ^ v to move, enter to play",MAGENTA_BLACK,A_BOLD);
    
        int input = wgetch(game_menu);

        // se mi trovo nel primo box  non posso salire
        if (input == KEY_UP && y > COORDINATA_Y_BOX_1){

            y -= VERTICAL_MOV_BOX;

        }else if (input == KEY_DOWN && y < COORDINATA_Y_SMALL_BOX){    // se mi trovo negli ultimi box non posso scendere ulteriormente

            y += VERTICAL_MOV_BOX;

        }

        // mi trovo nel blocco per iniziare una nuova partita
        if (y == COORDINATA_Y_BOX_1) {
            
            if (input == '\n') {
                flag = FALSE;
                menu.choice = TRUE;
            }

        }else if (y == COORDINATA_Y_BOX_2){  // mi trovo nel blocco per uscire

            if (input == '\n'){
                flag = FALSE;
                menu.choice = FALSE;
            }

        }

        // mi trovo nei blocchi per la scelta della modalita
        if (y >= COORDINATA_Y_SMALL_BOX) {

            if (input == KEY_RIGHT && (x >= COORDINATA_X_EASY && x < COORDINATA_X_HARD)){

                x += HORIZONTAL_MOV_BOX;

            }else if (input == KEY_LEFT && (x > COORDINATA_X_EASY && x <= COORDINATA_X_HARD)){

                x -= HORIZONTAL_MOV_BOX;

            }

            if (x == COORDINATA_X_EASY){ 

                if (input == '\n')
                {
                    menu.easy = TRUE;
                    menu.medium = FALSE;
                    menu.hard = FALSE;
                    flag = FALSE;
                }

            }else if (x == COORDINATA_X_MEDIUM){

                if (input == '\n')
                {
                    menu.medium = TRUE;
                    menu.easy = FALSE;
                    menu.hard = FALSE;
                    flag = FALSE;
                }

            }else if (x == COORDINATA_X_HARD){

                if (input == '\n')
                {
                    menu.hard = TRUE;
                    menu.medium = FALSE;
                    menu.easy = FALSE;
                    flag = FALSE;
                }
            }
        }

        wmove(game_menu, y, x); // Sposta il cursore alla nuova posizione

    }

    return menu;
}






