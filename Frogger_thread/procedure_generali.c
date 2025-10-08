#include "procedure_generali.h"
#include "macro_struct.h"



// Funzione che si occupa di inizializzare la grafica di gioco
void graphical_interface(Mode mode){

    bkgd(COLOR_PAIR(0)); // imposto lo sfondo_colori per stdscr
    refresh(); // forzo la stampa
    
    // Creo la finestra di gioco fixed_win
    fixed_win = newwin(HEIGHT_WINDOW, WIDTH_WINDOW, CENTER_Y, CENTER_X); // Creazione finestra principale
    wbkgd(fixed_win, COLOR_PAIR(WHITE_BLACK)); // imposto lo sfondo_colori per fixed_win


    wattron(fixed_win,COLOR_PAIR(RED_BLACK));
    //disegnamo i bordi verticali,spine
    for (int y = 0; y < HEIGHT_WINDOW ; y++) {
        mvwaddch(fixed_win, y, 0, '>');  
        mvwaddch(fixed_win, y, WIDTH_WINDOW - 1, '<');
    }
    wattroff(fixed_win,COLOR_PAIR(RED_BLACK));


    keypad(fixed_win , TRUE); // Abilito l'uso dei tasti speciali
    nodelay(fixed_win , TRUE); // Rende l'input dalla finestra `fixed_win` non bloccante: le funzioni come `wgetch` non aspettano che l'utente prema un tasto.
    

    // Finestra info-------------------------------------------------------------//
    
    info = newwin(HEIGHT_INFO, WIDTH_WINDOW, ORIGIN_Y, CENTER_X);

    wbkgd(info, COLOR_PAIR(WHITE_BLACK));
    box(info, 0, 0);
    nodelay(info,TRUE);
    keypad(info , TRUE); // Abilito l'uso dei tasti speciali
    
    // VALORI_INFO
    wattron(info, COLOR_PAIR(RED_BLACK));
    mvwprintw(info, DATI_INFO_Y, WIDTH_WINDOW-6, "TIME");
    mvwprintw(info, DATI_INFO_Y, LIVES_INFO_X, "LIVES:%d",mode.vite_rana);
    mvwprintw(info, DATI_INFO_Y, SCORE_INFO_X, "HI-SCORE:");
    wattroff(info, COLOR_PAIR(RED_BLACK));

    wrefresh(info);


    // PRATO VERDE------------------------------//
    //marciapiede di sopra
    wattron(fixed_win, COLOR_PAIR(RED_GREEN));
    for (int i=1;i < END_FIUME; i++) {
        for (int j = 1; j < WIDTH_WINDOW-1; j++) {
            sfondo_carattere[i][j]=' ';
            sfondo_colori[i][j] = COLOR_PAIR(RED_GREEN);
            mvwaddch(fixed_win, i, j, ' ');
        }
    }
    wattroff(fixed_win, COLOR_PAIR(RED_GREEN));

    //marciapiede di sotto
    wattron(fixed_win, COLOR_PAIR(RED_GREEN));
    for (int i=24;i < HEIGHT_WINDOW-1; i++) {
        for (int j = 1; j < WIDTH_WINDOW-1; j++) {
            sfondo_carattere[i][j]=' ';
            sfondo_colori[i][j] = COLOR_PAIR(RED_GREEN);
            mvwaddch(fixed_win, i, j, ' ');
        }
    }
    wattroff(fixed_win, COLOR_PAIR(RED_GREEN));


    // FIUME-----------------------------------//
    wattron(fixed_win, COLOR_PAIR(BLUE_BLUE));
    for ( int i = END_FIUME; i < HEIGHT_WINDOW-5; i++) {
        for (int j = 1; j < WIDTH_WINDOW-1; j++) {
            sfondo_carattere[i][j]=' ';
            sfondo_colori[i][j] = COLOR_PAIR(BLUE_BLUE);
            mvwaddch(fixed_win, i, j, ' ');                   
        }
    }
    wattroff(fixed_win, COLOR_PAIR(BLUE_BLUE));


    // TANE-----------------------------------------------------//
    wattron(fixed_win, COLOR_PAIR(BLUE_BLUE));
    for (int count = 0; count < NUM_HOLE; count++) {
        int x = HOLE_POSITION_X + count * (WIDTH_HOLE + ((WIDTH_WINDOW/10)-1));
        for (int i = 1; i <= 3; i++) { // Altezza dei blocchi
            for (int j = x; j < x + WIDTH_HOLE; j++) {
                sfondo_carattere[i][j]=' ';
                sfondo_colori[i][j] = COLOR_PAIR(BLUE_BLUE);
                mvwaddch(fixed_win, i, j, ' ');
            }
        }
    }
    wattroff(fixed_win, COLOR_PAIR(BLUE_BLUE));

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

// Funzione che si occupa di disegnare un singolo carattere con l'alternativa di mettere lo stesso in A_BOLD
void drawing(int y, int x, char carattere,int color,bool a_bold){

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

// Funzione che si occupa di inizializzare tutti i mutex e i semafori
void buffer_initialize() {
    
    // Inizializza gli indici del buffer circolare
    shared_buffer.in = 0;  // Indice dove i produttori inseriscono nuovi elementi
    shared_buffer.out = 0; // Indice da cui il consumatore legge gli elementi

    // Inizializza il mutex per proteggere l'accesso al buffer circolare , fa si che non ci siano accessi simultanei che potrebbero corrompere i dati
    if (pthread_mutex_init(&shared_buffer.mutex, NULL) != 0) {
        printf("Errore nell'inizializzazione del mutex , buffer\n");
        exit(1);
    }

    // Inizializza il semaforo che tiene traccia delle locazioni vuote nel buffer , all'inizio tutte le locazioni sono vuote
    if (sem_init(&shared_buffer.empty, 0, BUFFER_SIZE) != 0) {
        printf("Errore nell'inizializzazione del semaforo , buffer\n");
        exit(1);
    }

    // Inizializza il semaforo che tiene traccia delle locazioni vuote nel buffer , all'inizio non ci sono elementi nel buffer 
    if (sem_init(&shared_buffer.full, 0, 0) != 0) {
        printf("Errore nell'inizializzazione del semaforo , buffer\n");
        exit(1);
    }

    // Inizializza il mutex per proteggere l'accesso alle velocità dei flussi dei coccodrilli 
    if (pthread_mutex_init(&flow_speed_mutex, NULL) != 0) {
        printf("Errore nell'inizializzazione del mutex , velocita flussi\n");
        exit(1);
    }

    // Inizializza il mutex per proteggere l'accesso alle funzioni ncurses
    if (pthread_mutex_init(&ncurses_mutex, NULL) != 0) {
        printf("Errore nell'inizializzazione del mutex , ncurses\n");
        exit(1);
    }

    // Inizializza il semaforo che controlla il numero massimo di proiettili attivi (16) e di conseguenza viene utilizzato per andare a killare tutti i thread proiettile
    if (sem_init(&kill_bullet_sem, 0, NUM_COCCODRILLI) != 0) {
        printf("Errore nell'inizializzazione del semaforo , bullet\n");
        exit(1);
    }

    // Inizializza il semaforo che viene utilizzato per andare a killare la granata attiva 
    if (sem_init(&kill_granade_sem, 0, 1) != 0) {
        printf("Errore nell'inizializzazione del semaforo , granade\n");
        exit(1);
    }

    // Inizializza il semaforo utilizzato per il reset della manche
    if (sem_init(&reset_sem, 0, 0) != 0) {
        printf("Errore nell'inizializzazione del semaforo , reset_manche\n");
        exit(1);
    }

    // Inizializza il semaforo per gestire il movimento della rana quando la stessa è su un coccodrillo
    if (sem_init(&frog_on_croc_sem, 0, 1) != 0) {
        printf("Errore nell'inizializzazione del semaforo , frog_on_croc\n");
        exit(1);
    }

    // Inizializza il mutex per proteggere lo stato generale del gioco (pausa, fine partita, ecc.)
    if (pthread_mutex_init(&game_state_mutex, NULL) != 0) {
        printf("Errore nell'inizializzazione del mutex , game_state\n");
        exit(1);
    }

    // Inizializza il mutex per proteggere l'accesso alle coordinate degli oggetti 
    if (pthread_mutex_init(&coordinate_mutex, NULL) != 0) {
        printf("Errore nell'inizializzazione del mutex , coordinate\n");
        exit(1);
    }

    // Inizializza il mutex per proteggere l'accesso all'offset della rana rispetto al coccodrillo
    if (pthread_mutex_init(&offset_mutex, NULL) != 0) {
        printf("Errore nell'inizializzazione del mutex , offset\n");
        exit(1);
    }
   
}

// Funzione che si occupa di inserire aggiornamenti da parte dei produttori nel buffer
void produce_position(Obj *obj) {
    // Attendi uno slot vuoto nel buffer
    sem_wait(&shared_buffer.empty);
    
    // Blocca l'accesso al buffer
    pthread_mutex_lock(&shared_buffer.mutex);
    
    // Inserisci l'oggetto nel buffer
    shared_buffer.buffer[shared_buffer.in] = *obj;
    shared_buffer.in = (shared_buffer.in + 1) % BUFFER_SIZE;
    
    // Rilascia il mutex
    pthread_mutex_unlock(&shared_buffer.mutex);
    
    // Segnala uno slot pieno
    sem_post(&shared_buffer.full);
}

void game_music(){
    // Avvia la riproduzione in background della musica di gioco , in loop
    system("mpg123 -q --loop -1 sounds/songIntroAndMain.mp3 &");
}

void stop_music(){
    // Blocca la musica con SIGSTOP
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