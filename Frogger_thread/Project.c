#include "macro_struct.h"
#include "consumer.h"
#include "frog.h"
#include "granade.h"
#include "crocodile.h"
#include "bullet.h"
#include "menu.h"
#include "procedure_generali.h"



SharedBuffer shared_buffer; // Struttura che implementerà il buffer 


// Mutex
pthread_mutex_t game_state_mutex;  // Protegge l'accesso allo stato del gioco (pausa, timer, fine gioco)
pthread_mutex_t coordinate_mutex;  // Protegge l'accesso alle coordinate degli oggetti per evitare condizioni di gara
pthread_mutex_t ncurses_mutex;     // Protegge l'accesso alla finestra ncurses per evitare output concorrenti
pthread_mutex_t flow_speed_mutex;  // Protegge la gestione della velocità di scorrimento del fiume
pthread_mutex_t offset_mutex;      // Protegge l'accesso alla variabile che gestisce l'offset della rana sui coccodrilli

// Semafori utilizzati per la gestione del reset degli oggetti nel gioco
sem_t reset_sem;          // Indica la necessità di un reset generale degli oggetti rana e coccodrillo
sem_t kill_bullet_sem;    // Sincronizza la terminazione dei proiettili
sem_t frog_on_croc_sem;   // Indica quando la rana si trova sopra un coccodrillo
sem_t kill_granade_sem;   // Sincronizza la terminazione delle granate




int tempo = TEMPO;  // Variabile globale che tiene traccia del tempo di gioco

int crocodile_offset_x = 0;  // Offset orizzontale dei coccodrilli per gestire la posizione della rana nel coccodrillo stesso
int flow_speed[NUM_COCCODRILLI / 2] = {0}; // Array che memorizza la velocità di scorrimento per ciascuna riga di coccodrilli


bool game_running = TRUE; // Flag che indica se il gioco è in esecuzione
bool game_paused = FALSE; // Flag che indica se il gioco è in pausa


// Finestre ncurses
WINDOW *fixed_win, *info;  // 'fixed_win' è la finestra principale di gioco, 'info' mostra informazioni aggiuntive

// Matrici che memorizzano lo sfondo iniziale  della finestra di gioco
int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW];
char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW];




int main(){

    WINDOW *menu;
   
    
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

    
    

    // Cicla fin quando la variabile choice non verra settata a false
    while (start.choice) {



        game_running=TRUE;
        game_paused=FALSE;


        // Creazione strutture di base
        Obj frog;
        Obj crocodiles[NUM_COCCODRILLI];
        
        
        
        // Inizializzazione coordinate rana
        frog.coordinate_obj[0] = FROG_Y;
        frog.coordinate_obj[1] = FROG_X;

    
        pthread_t thread_id_crocodiles[NUM_COCCODRILLI]; // Array per gli ID dei thread coccodrillo
        C_Args *crocodile_args[NUM_COCCODRILLI];; // Array per gli argomenti dei thread coccodrillo


        pthread_t thread_id_rana; 
        F_Args *frog_args; // Argomenti del thread rana

        init_frog_args(&frog_args,&frog, mode,end_game);

        // Creazione del thread della rana
        if (pthread_create(&thread_id_rana, NULL, frog_thread, frog_args) != 0) {
            fprintf(stderr, "Errore nella creazione del thread della rana\n");
            free(frog_args);
            game_running = false;
            exit(1);
        }



        pthread_t thread_id_consumer;
        Mode *consumer_args; // Argomenti del thread consumatore

        // Allochiamo la memoria per il consumatore
        consumer_args = (Mode *)malloc(sizeof(Mode));
        if (consumer_args == NULL) {
            fprintf(stderr, "Errore nell'allocazione della memoria per gli argomenti del consumer\n");
            exit(1);
        }
        
        *consumer_args = mode; // Popola gli argomenti con i dati necessari


        // Creazione del thread consumatore 
        if (pthread_create(&thread_id_consumer, NULL, consumer_thread, consumer_args) != 0) {
            fprintf(stderr, "Errore nella creazione del thread consumatore\n");
            free(consumer_args);
            exit(1);
        }



        buffer_initialize(); // Inizializzazione buffer,mutex e semafori per la comunicazione tra thread

        pthread_mutex_lock(&ncurses_mutex);
        graphical_interface(mode); // Richiamo la graphical_interface
        pthread_mutex_unlock(&ncurses_mutex);

        crocodile_creation(mode,crocodile_args,crocodiles,thread_id_crocodiles); // creiamo i thread coccodrillo e inizializziamo gli argomenti
       
        
        // Terminazione thread
        kill_threads(thread_id_consumer, thread_id_rana, thread_id_crocodiles, consumer_args, crocodile_args, frog_args, end_game);
        
        // Cancello le due finestre
        pthread_mutex_lock(&ncurses_mutex);
        wclear(fixed_win);
        wclear(info);
        wrefresh(fixed_win);
        wrefresh(info);
        pthread_mutex_unlock(&ncurses_mutex);

        
        // Menu finale
        cursor_x = SPAWN_X, cursor_y = SPAWN_Y;
        flag = TRUE;
        start = main_menu(menu, cursor_x, cursor_y, flag, easy, medium, hard, end_game);
        mode = change_mode(start.easy, start.medium, start.hard);


    }

    

    // Termina la musica del gioco,le finestre,mutex,semafori e inoltre chiude ncurses
    end_music();
    mutex_semaphore_destroy();
    delwin(fixed_win);
    delwin(info);
    delwin(menu);
    endwin();

    exit(0);
}







