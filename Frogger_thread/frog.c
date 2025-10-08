#include "frog.h"
#include "macro_struct.h"
#include "granade.h"
#include "procedure_generali.h"



void draw_frog( int starty, int startx) {
    
    char frog[HEIGHT_FROG][WIDTH_FROG] = {
        {'o', '^', 'o'},
        {'!', '=', '!'}
    };
 

    // Disegna la rana carattere per carattere con colori diversi
    for (int i = 0; i < HEIGHT_FROG; i++) {
        for (int j = 0; j < WIDTH_FROG; j++) {

          
            if (i == 0 && j == 1) { 

                // Colore per la testa (il simbolo '^')
                drawing(starty+i,startx+j,frog[i][j],MAGENTA_BLACK,FALSE);

            } else if(i == 0 && (j == 0 || j == 2)){
                
                // Colore per gli "occhi"
                drawing(starty+i,startx+j,frog[i][j],WHITE_BLACK,FALSE);

            } else if (i == 1) { 
                
                // Colore per il corpo
                drawing(starty+i,startx+j,frog[i][j],MAGENTA_BLACK,FALSE);

            } 
        
       
        }
    }

}

// Funzione che si occupa di eliminare la posizione precedente della rana
void delete_prev_frog(int x, int y, bool in_tana ,int centro_tana) {


    for (int i = 0; i < HEIGHT_FROG; i++) {
        for (int j = 0; j < WIDTH_FROG; j++) {
        
            // Ripristina lo sfondo originale
            wattron(fixed_win, sfondo_colori[y + i][x + j]);
            mvwaddch(fixed_win, y + i, x + j, sfondo_carattere[y + i][x + j]);
            wattroff(fixed_win, sfondo_colori[y + i][x + j]);
            
        }
    }

    
    if (in_tana) {
      
        drawing(INIZIO_HOLE, centro_tana, 'X', RED_BLUE, FALSE);

    }
    

}

// Funzione che si occupa di gestire la rana
void* frog_thread(void* arg) {

    // Cast del puntatore all'argomento al tipo corretto
    F_Args *args = (F_Args *)arg;
    Obj *frog = args->frog;


    // Tane
    int x_inizio[NUM_HOLE]={0}; // Coordinata x di inizio tana
    int x_fine[NUM_HOLE]={0}; // Coordinata x di fine tana
    fill_hole(x_inizio,x_fine); 
    bool tane_visitate[NUM_HOLE]={false};
    

    // Produci la posizione iniziale
    produce_position(frog);
   

    int reset = 0;  // Variabile per tracciare il reset
    int grenade_shoot_id=0; // Contatore granate sparate 
    pthread_t thread_id_granade[MAX_GRANATE];


    while (game_running) {

        bool rana_in_tana=FALSE;

        
        pthread_mutex_lock(&ncurses_mutex);
        frog->ch = wgetch(fixed_win);
        pthread_mutex_unlock(&ncurses_mutex);

        
        // Gestisci i comandi di pausa/ripresa
        pthread_mutex_lock(&game_state_mutex);
        if (frog->ch == 'q' || frog->ch == 'Q') {
            game_paused = TRUE;
            stop_music();
            
        } else if (frog->ch == 'p' || frog->ch == 'P') {
            game_paused = FALSE;
            resume_music();
        }
        pthread_mutex_unlock(&game_state_mutex);
        
        
        if (!game_paused) {

            // Proteggi l'accesso alle coordinate
            pthread_mutex_lock(&coordinate_mutex);
            
            // Salva le vecchie coordinate
            int frog_x = frog->coordinate_obj[1];
            int frog_y = frog->coordinate_obj[0];
            
            // Salviamo il valore corrente del semaforo
            int offset_sem_value;
            sem_getvalue(&frog_on_croc_sem,&offset_sem_value);

            // Se il valore del semaforo e' uguale a 0 , il consumatore sta avvisando che la posizione del coccodrillo si e' aggiornata 
            if(offset_sem_value==0){

                sem_post(&frog_on_croc_sem); 
                frog->coordinate_obj[1]=crocodile_offset_x; // Di conseguenza la frog si aggancia al coccodrillo 

            }
        
            // Aggiorna le coordinate in base all'input
            if (frog->ch == KEY_UP && frog_y > UPPER_LIMIT) {

                frog->coordinate_obj[0] -= VERTICAL_MOV;
                
            }else if (frog->ch == KEY_DOWN && frog_y < HEIGHT_WINDOW - LOWER_LIMIT) {

                frog->coordinate_obj[0] += VERTICAL_MOV;

            }else if (frog->ch == KEY_LEFT && frog_x > LEFT_LIMIT) {

                if (frog_x % 2 == 0) {
                    frog->coordinate_obj[1] -= 1;
                } else {
                    frog->coordinate_obj[1] -= HORIZONTAL_MOV;
                }
              
            }else if (frog->ch == KEY_RIGHT && frog_x < WIDTH_WINDOW - RIGHT_LIMIT) {
                
                if (frog_x % 2 == 0) {
                    frog->coordinate_obj[1] += 1;
                } else {
                    frog->coordinate_obj[1] += HORIZONTAL_MOV;
                }
                
            }
            // Se si preme space , il contatore di granate generate e' minore di 10 e la rana non si trova nel marciapiede superiore , le granate vengono generate
            else if(frog->ch == ' ' && grenade_shoot_id < MAX_GRANATE && frog->coordinate_obj[0] > END_FIUME-2){
                
                granade_args(&grenade_shoot_id, frog, thread_id_granade);

            }


            pthread_mutex_unlock(&coordinate_mutex);


            // Controlliamo il valore corrente del semaforo di reset della rana
            int reset_sem_value;
            sem_getvalue(&reset_sem,&reset_sem_value);

            // Se il valore del semaforo e' maggiore di 0 allora il reset da parte del consumatore e' avvenuto
            if ( reset_sem_value > 0) {

                reset_frog(&reset, frog, x_inizio, x_fine, tane_visitate, &rana_in_tana, &grenade_shoot_id);

            }else{

                sem_init(&kill_granade_sem,0,1); 
                reset = 0;  // Resetta il flag quando il semaforo non è attivo
                frog->tana_visitata=FALSE; // Resettiamo il campo di visita della tana

            }
            
            // Aggiorniamo la posizione della rana
            produce_position(frog);
            
        }
    
        
        usleep(15000);
    }
    
    
    return NULL;
}

// Funzione che popola il vettore di inizio e di fine delle tane
void fill_hole(int x_inizio[], int x_fine[]){
    int pos_x_inizio = HOLE_POSITION_X; // Coordinata X iniziale della prima tana
    int pos_x_fine = 10;                // Coordinata X finale della prima tana

    // Popolo il vettore che conterra' le x di inizio di ogni tana 
    for (int i = 0; i < NUM_HOLE; i++){
        x_inizio[i] = pos_x_inizio;  // Salva la posizione iniziale della tana corrente
        pos_x_inizio += 12;          // Incrementa la posizione per la tana successiva
    }

    // Popolo il vettore che conterra' le x di fine di ogni tana 
    for (int i = 0; i < NUM_HOLE; i++){
        x_fine[i] = pos_x_fine;  // Salva la posizione finale della tana corrente
        pos_x_fine += 12;        // Incrementa la posizione per la tana successiva
    }
}

// Funzione che si occupa di gestire l' entrata o meno in tana 
void manage_entrance_hole(Obj *obj,int x_inizio[NUM_HOLE], int x_fine[NUM_HOLE], bool tane_visitate[NUM_HOLE],bool*rana_in_tana) {


    int tana_corrente = -1; // Indice della tana in cui si trova la rana
    int tanaCenter = 0;     // Coordinata X centrale della tana corrente

    // Verifica per ogni tana se la coordinata x della rana è compresa tra x_inizio e x_fine
    for (int i = 0; i < NUM_HOLE; i++) {
       
        if  (obj->coordinate_obj[1] >= x_inizio[i] && (obj->coordinate_obj[1]+2) <= x_fine[i]) {
            
            *rana_in_tana = TRUE;
            tana_corrente = i;  // Salva l'indice della tana corrente
            tanaCenter = x_inizio[i] + (WIDTH_HOLE / 2); // Calcola il centro della tana
            obj->tana_visitata=TRUE; 
            obj->centro_tana=tanaCenter; // Salvo la posizione centrale della tana(che mi servira' nella stampa della X quando la rana entra in tana)

        }
        
    }

    // Se la rana è in una tana
    if (*rana_in_tana == TRUE) {

        system("aplay sounds/sound-frogger-coin-in.wav > /dev/null 2>&1 &"); 

        // Controlla se la tana non è già stata visitata
        if (tane_visitate[tana_corrente] == FALSE) {
        
            tane_visitate[tana_corrente] = TRUE;  // Segna la tana come visitata
            obj->score += 20+(tempo); // Aggiunge punti al punteggio totale
            obj->tane_visitate++; // Incrementa il contatore delle tane visitate
            
        } 

        // Se la tana è già stata visitata, la rana perde una vita e punti
        else {

            obj->vite--;
            
            if(obj->score>0){
                obj->score -= 10;
            }

        }


        // Resetta il timer per indicare il completamento del round
        pthread_mutex_lock(&game_state_mutex);
        tempo=TEMPO;
        pthread_mutex_unlock(&game_state_mutex);

    }
    else {

        obj->vite--;
        if(obj->score>0){
            obj->score -= 10;
        }
        
    }
   
   
}

// Funzione che iniziliazza gli argomenti per il thread rana
void init_frog_args(F_Args **frog_args, Obj *frog, Mode game_mode,int end_game[2]) {

    // Verifica che il puntatore agli argomenti non sia NULL
    *frog_args=malloc(sizeof(F_Args));
    if (*frog_args == NULL) {
        fprintf(stderr, "Errore: argomenti della rana sono NULL\n");
        exit(1);
    }
    
    // Inizializzazione dei campi della rana
    frog->vite = game_mode.vite_rana;
    frog->id = 1;
    frog->score = 0;
    frog->tipo = RANA;
    frog->tane_visitate = 0;
    frog->thread_id = pthread_self();
    frog->centro_tana=0;
    frog->tana_visitata=FALSE;

    for (int i = 0; i < 2; i++){
        frog->end_game[i]=end_game[i];
    }
    
    
    // Inizializzazione dello sprite
    frog->sprite_frog[0][0] = 'o';
    frog->sprite_frog[0][1] = '^';
    frog->sprite_frog[0][2] = 'o';
    frog->sprite_frog[1][0] = '!';
    frog->sprite_frog[1][1] = '=';
    frog->sprite_frog[1][2] = '!';
    
    // Assegnazione dei valori alla struttura degli argomenti
    (*frog_args)->frog = frog;
    (*frog_args)->game_mode = game_mode;
}

// Funzione per quando un proiettile collide con la rana
void check_frog_bullet_collision(Obj *frog, Obj bullet[], bool * collision_bullet_frog) {
    
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        if (frog->coordinate_obj[0] == bullet[i].coordinate_obj[0] &&
            (frog->coordinate_obj[1] == bullet[i].coordinate_obj[1] || (frog->coordinate_obj[1] + 2) == bullet[i].coordinate_obj[1])) {

            // Collisione rilevata, la rana perde la manche
            system("aplay sounds/bullet_to_frog.wav > /dev/null 2>&1 &");
            
            *collision_bullet_frog = TRUE;

            pthread_mutex_lock(&game_state_mutex);
            tempo=TEMPO;
            pthread_mutex_unlock(&game_state_mutex);


        }
    }
}

// Funzione per controllare che la rana si trovi sopra un coccodrillo
void check_frog_on_crocodile(Obj *obj, Obj crocodiles[NUM_COCCODRILLI], bool *frog_on_crocodile,int *crocodile_index) {
   
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        // Controllo se la rana si trova nel coccodrillo
        if (obj->coordinate_obj[0] == crocodiles[i].coordinate_obj[0] &&
            (obj->coordinate_obj[1] >= crocodiles[i].coordinate_obj[1] &&
             obj->coordinate_obj[1] < (crocodiles[i].coordinate_obj[1] + (WIDTH_CROC - 1)))) {

            // Se la rana si trova sopra il coccodrillo
            // Controlliamo se la stessa si trova entro i limiti del fiume
            if (obj->coordinate_obj[1] >= 0 && obj->coordinate_obj[1] <= WIDTH_WINDOW - 1) {
                
                // Se la rana si trova all'interno del fiume settiamo la variabile a true
                // Salviamo il coccodrillo in cui la rana si trova ora
                
                *frog_on_crocodile = TRUE;
                *crocodile_index = i;

            } 
            

        }
    }

}

void is_first_time_on_crocodile(Obj *obj, Obj crocodiles[NUM_COCCODRILLI], int *crocodile_index,bool *isFirstTimeOnCroc, int *id, int *posX) {
   
    // Calcola la posizione relativa della rana rispetto al coccodrillo
    *posX= obj->coordinate_obj[1] - crocodiles[*crocodile_index].coordinate_obj[1];

    // Aggiorna l'ID del coccodrillo su cui si trova la rana
    *id = crocodiles[*crocodile_index].id;

    // Disattiva il flag 
    *isFirstTimeOnCroc = FALSE;
    
}

void frog_already_on_croc(Obj *obj , Obj crocodiles[NUM_COCCODRILLI], int crocodile_index, int *posX){

    // Controllo se la rana cade dal bordo del fiume, quando tocca i bordi vado a modificare l'offset della rana sopra il coccodrillo
    if (obj->coordinate_obj[1] == LEFT_LIMIT && crocodiles[crocodile_index].direction_cocc == TRUE) {
        *posX = obj->coordinate_obj[1] - crocodiles[crocodile_index].coordinate_obj[1];
    }
    if (obj->coordinate_obj[1] == (WIDTH_WINDOW - RIGHT_LIMIT) && crocodiles[crocodile_index].direction_cocc == FALSE) {
        *posX = obj->coordinate_obj[1] - crocodiles[crocodile_index].coordinate_obj[1];
    }

    // Se la rana si trova già sullo stesso coccodrillo, può muoversi
    switch (obj->ch) {
        case KEY_LEFT:
            if(obj->coordinate_obj[1] > LEFT_LIMIT){
                *posX -= HORIZONTAL_MOV;
            }
        break;

        case KEY_RIGHT:
            if(obj->coordinate_obj[1] < WIDTH_WINDOW - RIGHT_LIMIT){
                *posX += HORIZONTAL_MOV;
            }
        break;
    
    }

}

void frog_out_croc(bool *isFirstTimeOnCroc, Obj *obj,Obj crocodiles[NUM_COCCODRILLI], bool *drop_on_the_river){

    // se la rana non si trova nel coccodrillo , allora setto questa variabile a true
    // facendo questo faccio si che se poi la rana sale su un coccodrillo , setto l offset e le posizioni iniziali
    *isFirstTimeOnCroc = TRUE;
    *drop_on_the_river = FALSE;

    // se la rana si trova all interno del fiume
    if (obj->coordinate_obj[0] >= END_FIUME && obj->coordinate_obj[0] <= START_FIUME){

        // ciclo su tutti i coccodrilli e vedo se la rana si trova all esterno del coccodrillo
        for (int i = 0; i < NUM_COCCODRILLI; i++){

            if (obj->coordinate_obj[0] == crocodiles[i].coordinate_obj[0] && (obj->coordinate_obj[1] + 1 < (crocodiles[i].coordinate_obj[1])) || (obj->coordinate_obj[1] + 1 > (crocodiles[i].coordinate_obj[1]) + (WIDTH_CROC - 1))){

                // se la rana non si trova piu sul coccodrillo corrente, si perde la manche
                *drop_on_the_river = TRUE;
            
            }
        }

        if (*drop_on_the_river == TRUE){
            system("aplay sounds/water-splash.wav > /dev/null 2>&1 &");

            pthread_mutex_lock(&game_state_mutex);
            tempo=TEMPO;
            pthread_mutex_unlock(&game_state_mutex);
        }
    }

}

// Funzione che resetta i campi della rana in caso di fine manche
void reset_frog(int* reset, Obj* frog, int x_inizio[], int x_fine[], bool tane_visitate[], bool* rana_in_tana, int* grenade_shoot_id) {

    if (*reset == 0) {
        // Reset granate
        int granade_sem_value;
        sem_getvalue(&kill_granade_sem, &granade_sem_value);

        while (granade_sem_value > 0) {
            sem_trywait(&kill_granade_sem);
            sem_getvalue(&kill_granade_sem, &granade_sem_value);
        }

        // Gestione reset della rana
        if (frog->coordinate_obj[0] <= HOLE_HEIGHT) {

            manage_entrance_hole(frog, x_inizio, x_fine, tane_visitate, rana_in_tana);
            *rana_in_tana = FALSE;

        } else {

            if (frog->score > 0) {
                frog->score -= 10;
            }
            frog->vite--;
        }
        
        // Reset posizione della rana
        frog->coordinate_obj[0] = FROG_Y;
        frog->coordinate_obj[1] = FROG_X;

        pthread_mutex_lock(&coordinate_mutex);
        if(frog->vite>0 && frog->tane_visitate==NUM_HOLE){
            frog->score+=(frog->vite*50);
        }
        pthread_mutex_unlock(&coordinate_mutex);

        // Aspetta qualche millisecondo per garantire il reset
        usleep(100000);
        sem_trywait(&reset_sem);
        // Reset del numero di granate
        *grenade_shoot_id = 0; 
        *reset = 1; // indichiamo che il reset e' avvenuto 
    }
}
