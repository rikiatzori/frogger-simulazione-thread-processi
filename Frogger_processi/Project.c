


#include "macro_struct.h"
#include "father.h"
#include "frog.h"
#include "granade.h"
#include "crocodile.h"
#include "bullet.h"
#include "menu.h"
#include "procedure_generali.h"



// variabile che indica il tempo per ogni manche
int tempo = TEMPO;





int main(){

    WINDOW *fixed_win, *info, *menu;
    /*
    Dichiarazione dei puntatori a tre finestre:
      - 'fixed_win' per la finestra principale del gioco;
      - 'info' per la finestra informativa;
      -  'menu' gestire il gioco e le sue modalita.
    */

    srand(time(NULL));
    // Inizializza il generatore di numeri casuali con il valore corrente del tempo, per ottenere numeri pseudo-casuali diversi a ogni esecuzione del programma.

    initscr();
    // Inizializza la modalità `ncurses`, configurando il terminale per la gestione della grafica testuale.

    noecho();
    // Disabilita l'eco dei tasti premuti sul terminale. Questo è utile per i giochi, poiché non vuoi che l'input dell'utente venga visualizzato.

    cbreak();
    // Disabilita il buffering dell'input, permettendo di leggere i tasti premuti immediatamente, senza attendere un `\n`.

    curs_set(FALSE);
    // Nasconde il cursore sullo schermo, utile per mantenere l'aspetto grafico del gioco senza distrazioni.

    start_color();
    // Abilita l'uso dei colori nella modalità `ncurses`. Necessario prima di definire le combinazioni di colori (`COLOR_PAIR`).

    create_colors();
    // Abilita tutti i colori inizializzati e le coppie colore.

    game_music();
    // Abilita  la musica del gioco

    // Creazione finestra______________________________________________//
    menu = newwin(HEIGHT_WINDOW, WIDTH_WINDOW, CENTER_Y, CENTER_X);
    // Abilito l uso dei tasti direzione nel menu
    keypad(menu, TRUE);
    // Imposto lo sfondo per il menu
    wbkgd(menu, COLOR_PAIR(GREEN_BLACK));
    //________________________________________________________________//

    // Creazione menu di gioco e modalita_____________________________________________________________________//
    // Posizione iniziale del cursore
    int cursor_x = SPAWN_X, cursor_y = SPAWN_Y;
    // Variabile che fara ciclare il while che allo stesso tempo ci permettera di interfacciarci con il menu
    bool flag = TRUE;

    // modalita di gioco
    bool easy = FALSE;
    bool medium = FALSE;
    bool hard = FALSE;

    Mode mode;  // Struttura per l impostazione delle modalita
    Menu start; // Struttura per iniziare  o terminare la partita e selezionare le modalita

    // Con ERR indichiamo che la partita non e ancora iniziata e quindi non effettuiamo le stampe finali
    // Questo vettore conterra i dati di fine partita(score e vite)
    int end_game[2];
    end_game[0] = ERR;
    end_game[1] = ERR;

    // Rende la struttura che fara o meno iniziare la partita e scegliere le modalita
    start = main_menu(menu, cursor_x, cursor_y, flag, easy, medium, hard, end_game);

    // Setta i valori in base alla variabile booleana scelta
    mode = change_mode(start.easy, start.medium, start.hard);

   
    //_______________________________________________________________________________________________________//

    // Cicla fin quando la variabile choise non verra settata a false
    while (start.choice){

        // Creazione struttura di base
        Obj obj;

        // Vettore per memorizzare tutti i pid dei processi coccodrilli
        int pids_croc[NUM_COCCODRILLI];

        // Matrice per cancellazione sfondo
        int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW];
        char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW];

        // Variabili che servono per la gestione della rana
        int frog_y = FROG_Y;
        int frog_x = FROG_X;

        

        // Richiamo la graphical_interface
        graphical_interface(&fixed_win, &info, sfondo_colori, sfondo_carattere, mode);

        // Disegno rana
        draw_frog(fixed_win, frog_y, frog_x);

        // Creo e controllo se la creazione della pipe e andata a buon fine____________________________________//
        int p_childs_to_father[2]; // Pipe childs to father
        int p_father_to_frog[2];   // Pipe father to frog
        if (pipe(p_childs_to_father) == -1 || pipe(p_father_to_frog) == -1){
            perror("Error");
            exit(1);
        }


        int p_father_to_croc[NUM_COCCODRILLI][2]; // Array che conterra una pipe per ogni PROCESSO coccodrillo
        for (int i = 0; i < NUM_COCCODRILLI; i++){
            if (pipe(p_father_to_croc[i]) == -1){
                perror("Error");
                exit(1);
            }
        }
        //____________________________________________________________________________________________________//

        // Creazione processi_______________________________________//
        pid_t pid_frog = fork();
        
        if (pid_frog < 0){
            perror("Fork err");
            exit(2);
        }
        
        if (pid_frog == 0){

            // Sono il figlio
            // Funzione rana
            frog_process(p_childs_to_father, p_father_to_frog, &fixed_win, mode);
        }else{
            //creazione delle coppie dei coccodrilli
            crocodile_creation(p_childs_to_father,p_father_to_croc,pids_croc,mode);
           
        }
        //___________________________________________________________//

        // Funzione padre
        father_process(&fixed_win, &info, sfondo_colori, sfondo_carattere, p_childs_to_father, p_father_to_frog, p_father_to_croc, pid_frog, pids_croc, obj, end_game, mode);

        // Cancello le due finestre
        wclear(fixed_win);
        wclear(info);
        wrefresh(fixed_win);
        wrefresh(info);

        // Menu finale_____________________________________________________________//
        // Reset delle coordinate del cursore
        cursor_x = SPAWN_X, cursor_y = SPAWN_Y; // Posizione iniziale del cursore
        // Reset della variabile per il ciclo principale del menu
        flag = TRUE;

        start = main_menu(menu, cursor_x, cursor_y, flag, easy, medium, hard, end_game);

        mode = change_mode(start.easy, start.medium, start.hard);
    
    }

    // Termina il processo per la musica del gioco e le finestre , inoltre chiude ncurses
    end_music();
    delwin(fixed_win);
    delwin(info);
    delwin(menu);
    endwin();

    exit(0);
}













