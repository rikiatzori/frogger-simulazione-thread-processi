#include "crocodile.h"
#include "macro_struct.h"
#include "bullet.h"
#include "procedure_generali.h"

// Funzione che disegna il coccodrillo
void draw_coccodrille(int starty, int startx, bool verso_flusso) {

    // Sprite del coccodrillo
    char crocodile[HEIGHT_CROC][WIDTH_CROC] = {
        {'<','0','=','=','=','=','=','|'}, // Prima riga della sprite
        {'<','0','=','=','=','=','=','|'}  // Seconda riga della sprite
    };    
    
   
    // Disegna il coccodrillo carattere per carattere
    for (int i = 0; i < HEIGHT_CROC; i++) {       // Cicla sulle righe della sprite
        
        for (int j = 0; j < WIDTH_CROC; j++) {    // Cicla sulle colonne della sprite

            // Carattere testa del coccodrillo 
            if (j == 0) { 

                if (verso_flusso ) { 

                    // Verso normale (da sinistra a destra) , disegno la testa
                    drawing(starty+i,startx+j,crocodile[i][j], RED_BLACK, TRUE);
                }else { 
                    // Verso speculare (da destra a sinistra) , disegno la coda
                    drawing( starty+i, startx+j ,'|', GREEN_BLACK, TRUE);

                }
                    
            }

            // Carattere occhi del coccodrillo 
            else if (j == 1) {

                if (verso_flusso ) { 
                   // Verso normale (da sinistra a destra) disegno gli occhi
                    drawing(starty+i, startx+j ,crocodile[i][j], WHITE_BLACK, TRUE);

                }else { 

                    // Verso speculare (da destra a sinistra) disegno il corpo
                    drawing(starty+i, startx+j ,'=', GREEN_BLACK, TRUE);
                }

            }else { // Corpo del coccodrillo 

                if (verso_flusso ) { 
                    // Verso normale (da sinistra a destra) disegno il corpo
                    drawing(starty+i, startx+j ,crocodile[i][j], GREEN_BLACK, TRUE);

                }else { 
                		 // Verso speculare (da destra a sinistra) disegno il corpo
                    if (j > 0 && j < WIDTH_CROC-2) { 

                        drawing( starty+i, startx+j ,'=', GREEN_BLACK, TRUE);
                        
                    }

                    if (j == WIDTH_CROC-2) { // Disegno l occhio
                       
                        drawing( starty+i, startx+j ,'0', WHITE_BLACK, TRUE);
                    }

                    if (j == WIDTH_CROC-1) { // Disegno la testa speculare
                       
                        drawing( starty+i, startx+j ,'>', RED_BLACK, TRUE);
                    }
                }
            }
        }

    }

}

// Funzione che cancella la posizione precedente del coccodrillo
void delete_prev_croc( int x, int y) {

    // Cancella l'area occupata dal coccodrillo dalla posizione precedente
    for (int i = 0; i < HEIGHT_CROC; i++) {
        for (int j = 0; j < WIDTH_CROC; j++) {
		  
			
		int current_x = x + j ;
		int current_y = y + i;


		// Ripristina il colore nero ai bordi
		if (current_x <= 0 || current_x == WIDTH_WINDOW - 1) {
		 

		 drawing( current_y, current_x ,' ', COLOR_PAIR(COLOR_BLACK), FALSE);


		 // Disegnamo i bordi verticali,spine
		 wattron(fixed_win,COLOR_PAIR(RED_BLACK));

		 for (int y = 0; y < HEIGHT_WINDOW ; y++) {
			mvwaddch(fixed_win, y, 0, '>');  // Bordo sinistro con spine 
			mvwaddch(fixed_win, y, WIDTH_WINDOW - 1, '<');  // Bordo destro con spine 
		 }
		 
		 wattroff(fixed_win,COLOR_PAIR(RED_BLACK));
		 
		 
		} else {	// Ripristina il colore del fiume se il coccodrillo non è arrivato ai bordi
		 
		 drawing( current_y, current_x ,' ', BLUE_BLUE, FALSE);
		 
		}
		}
    }
}

// Funzione che si occupa di gestire i threads coccodrilli
void *crocodile_thread(void *arg) {

    // Cast del puntatore all'argomento al tipo corretto
    C_Args *args = (C_Args *)arg;
    Obj *crocodile = args->crocodile; // Oggetto coccodrillo da gestire
    Mode current_mode = args->game_mode;  // Modalità di gioco corrente
    
    // Memorizza le coordinate iniziali del coccodrillo
    int cocc_x = crocodile->coordinate_obj[1];
    int cocc_y = crocodile->coordinate_obj[0];

    // Variabili per la gestione dei proiettili
    bool flag_bullet = FALSE; // Flag per indicare se un proiettile è attivo
    int intervallo_sparo = (rand() % MIN) + current_mode.t_period_spawn_bullet; // Calcola un intervallo casuale per lo sparo in base alla modalita selezionata
    time_t next_bullet_time = time(NULL) + intervallo_sparo;  // Imposta il momento del prossimo sparo
    
    int reset = 0; // Flag per indicare se il reset è stato già gestito
    pthread_t thread_id_bullet; // Id del thread del proiettile
    B_Args* bullet_args = NULL; // Argomenti per il thread del proiettile
    
    // Produci la posizione iniziale
    produce_position(crocodile);

    while(game_running) {

        time_t live_time = time(NULL);

        // Controlla lo stato del semaforo di reset
        int value;
        sem_getvalue(&reset_sem,&value);
        

        // Gestione reset coccodrillo quando il semaforo è attivo
        crocodile_reset(crocodile,&reset,value);



        // Gestione movimento e creazione proiettili se il gioco non è in pausa
        if (!game_paused) {
            
                        
            pthread_mutex_lock(&coordinate_mutex); // Protegge l'accesso alle coordinate con mutex

            if (crocodile->direction_cocc) {

                // Movimento verso sinistra
                crocodile->coordinate_obj[1] -= 1;

                // Reset posizione se raggiunge il bordo
                if (crocodile->coordinate_obj[1] <= -WIDTH_CROC) {
                    crocodile->coordinate_obj[1] = WIDTH_WINDOW + 10; // Riposiziona a destra dello schermo
                }

            } else {

                // Movimento verso destra
                crocodile->coordinate_obj[1] += 1;

                // Reset posizione se raggiunge il bordo
                if (crocodile->coordinate_obj[1] >= WIDTH_WINDOW + WIDTH_CROC) {
                    crocodile->coordinate_obj[1] = -10; // Riposiziona a sinistra dello schermo
                }
            }

            // Aggiorna posizione grafica se cambiata
            if (cocc_x != crocodile->coordinate_obj[1]) {
                produce_position(crocodile);
                cocc_x = crocodile->coordinate_obj[1];
            }
            
            // Gestione creazione proiettile e inizializzazione argomenti 
            if (live_time >= next_bullet_time && !flag_bullet) {
               
                init_bullet_args(crocodile, current_mode, &thread_id_bullet,&bullet_args, &flag_bullet, &next_bullet_time, live_time);
            }
            

            pthread_mutex_unlock(&coordinate_mutex);
        }
        
        // Velocità del coccodrillo in base alla modalità
        usleep(crocodile->speed_cocc);
    }

    
    
    return NULL;
}

// Funzione che si occupa di inizializzare gli argomenti dei coccodrilli
void init_crocodile_args(C_Args **crocodile_args, Obj crocodiles[], int index, int speed, int direction,int cocc_x,int cocc_y, Mode game_mode) {
    
     // Salva l'ID originale (base 1)
    int id = index;
    
    // Converte l'indice in base 0 per l'accesso agli array
    index -= 1;
    
    // Alloca memoria per gli argomenti del thread del coccodrillo
    crocodile_args[index] = (C_Args *)malloc(sizeof(C_Args));
    if (!crocodile_args[index]) {
        // Gestione dell'errore di allocazione della memoria
        perror("Errore nell'allocazione di crocodile_args");
        exit(EXIT_FAILURE);
    }
    
    // Inizializza le coordinate del coccodrillo
    crocodiles[index].coordinate_obj[0] = cocc_y;
    crocodiles[index].coordinate_obj[1] = cocc_x;
    
    // Inizializza le proprietà base del coccodrillo
    crocodiles[index].direction_cocc = direction;  // Direzione del movimento
    crocodiles[index].id = id;                     // ID del coccodrillo (base 1)
    crocodiles[index].speed_cocc = speed;          // Velocità del movimento
    crocodiles[index].tipo = COCCODRILLO;          // Tipo dell'oggetto
    crocodiles[index].thread_id = pthread_self();  // ID del thread corrente
    
    // Inizializza lo sprite grafico del coccodrillo
    // Prima riga dello sprite (vista frontale superiore)
    crocodiles[index].sprite_cocc[0][0] = '<';
    crocodiles[index].sprite_cocc[0][1] = '0';
    crocodiles[index].sprite_cocc[0][2] = '=';
    crocodiles[index].sprite_cocc[0][3] = '=';
    crocodiles[index].sprite_cocc[0][4] = '=';
    crocodiles[index].sprite_cocc[0][5] = '=';
    crocodiles[index].sprite_cocc[0][6] = '=';
    crocodiles[index].sprite_cocc[0][7] = '|';
    
    // Seconda riga dello sprite (vista frontale inferiore)
    crocodiles[index].sprite_cocc[1][0] = '<';
    crocodiles[index].sprite_cocc[1][1] = '0';
    crocodiles[index].sprite_cocc[1][2] = '=';
    crocodiles[index].sprite_cocc[1][3] = '=';
    crocodiles[index].sprite_cocc[1][4] = '=';
    crocodiles[index].sprite_cocc[1][5] = '=';
    crocodiles[index].sprite_cocc[1][6] = '=';
    crocodiles[index].sprite_cocc[1][7] = '|';
    
    // Associa il coccodrillo agli argomenti del thread
    crocodile_args[index]->crocodile = &crocodiles[index];
    crocodile_args[index]->game_mode = game_mode;

    
   
    
}

// Funzione che si occupa di controllare la collisione tra proiettile e coccodrillo(gestione grafica)
void collision_bullet_to_croc(Obj *bullet, int id_bullet, Obj *crocodiles, bool *collision_bullet) {
    

    // Itera attraverso tutti i coccodrilli per verificare se c'è una collisione con il proiettile
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        
        if (bullet[id_bullet].coordinate_obj[0] == crocodiles[i].coordinate_obj[0] &&
            bullet[id_bullet].coordinate_obj[1] >= crocodiles[i].coordinate_obj[1] &&
            bullet[id_bullet].coordinate_obj[1] < crocodiles[i].coordinate_obj[1] + WIDTH_CROC) {
            
            *collision_bullet = TRUE; // Imposta il flag di collisione a TRUE
            
            
            draw_coccodrille(crocodiles[i].coordinate_obj[0],crocodiles[i].coordinate_obj[1],crocodiles[i].direction_cocc);
        }
    }

    // Se non è stata rilevata alcuna collisione con i coccodrilli
    if (!*collision_bullet) {
        // Cancella la vecchia posizione del proiettile 
        drawing(bullet[id_bullet].coordinate_obj[0], bullet[id_bullet].coordinate_obj[1], ' ', BLUE_BLUE, FALSE);
    }

}

// Funzione che inizializza l array dei coccodrilli nel padre
void fill_crocodile(Obj crocodiles[NUM_COCCODRILLI], Obj bullet[NUM_COCCODRILLI]){

    // Inizializziamo il vettore
    for (int i = 0; i < NUM_COCCODRILLI; i++){
        crocodiles[i].coordinate_obj[0] = 0;
        crocodiles[i].coordinate_obj[1] = 0;

        bullet[i].coordinate_obj[0] = -1;
        bullet[i].coordinate_obj[1] = -1;
    }
}

// Funzione che si occupa di creare i 16 processi coccodrilli
void crocodile_creation(Mode mode, C_Args *crocodile_args[NUM_COCCODRILLI],Obj crocodiles[NUM_COCCODRILLI],pthread_t thread_id_crocodiles[NUM_COCCODRILLI]){

    // Variabili che servono per la gestione dei coccodrilli
    int cocc_y= CROC_Y; //posizione in altezza del primo flusso
    int cocc_x=0;
    int cocc_id=0;
    // Ad ogni partita avremo una direzione di spaw diversa
    bool direction= rand() % 2;     // TRUE = SINISTRA | FALSE = DESTRA
    // Salviamo la direzione del precedente flusso
    bool before_direction;
    // Velocita del flusso
    int speed=0;
    // Velocita generata in base alla modalita
    int speed_generated=0;

	
	
        // Creo 8 coppie di coccodrillo , 2 per ogni flusso
         // Creo 8 coppie di coccodrillo , 2 per ogni flusso
         for (int i = 0; i < NUM_FLUSSI; i++) {

            // Aggiorniamo la direzione
            before_direction = direction;
            
            // Generiamo la velocita in base alla modalita selezionata
            speed_generated = mode.speed_crocodile_min + rand() % ( mode.speed_crocodile_max -	mode.speed_crocodile_min + 1 );
                
            // Aggiorniamo la velocita dei flussi
            speed = speed_generated;  

            // per ogni coccodrillo della coppia
            for(int j= 0; j < 2; j++){

        
                // Incremento il suo id, il primo coccodrillo quindi avra id==1
                cocc_id++;
        
                // Se sto creando il primo coccodrillo della coppia
                if (j == FIRST_OF_THE_COUPLE){
                
                    // Lo posiziono nella prima parte della finestra
                    cocc_x=  rand() % 25;
                
                }

                // Se sto creando il secondo coccodrillo della coppia
                else if (j == SECOND_OF_THE_COUPLE){

                    // Lo posiziono nella seconda meta della finestra
                    cocc_x=  rand() % (WIDTH_WINDOW - 35 + 1) + 35;

                }



                // Inizializza gli argomenti del coccodrillo
                init_crocodile_args(crocodile_args, crocodiles, cocc_id, speed, direction, cocc_x, cocc_y,mode);

                // Crea il thread coccodrillo
                if (pthread_create(&thread_id_crocodiles[cocc_id-1], NULL, crocodile_thread, crocodile_args[cocc_id-1]) != 0) {
                    fprintf(stderr, "Errore nella creazione del thread coccodrillo %d\n", cocc_id);
                    free(crocodile_args[cocc_id-1]);
                    exit(1);
                }



            }
        
            // Sposto la coordinata y della prossima coppia da generare in su di due caratteri
            cocc_y -= 2;

            // Controllo se la direzione del flusso precedente 
            if (before_direction ) {
                //  Se era true allora imposto la corrente a false
                direction = FALSE;
            } else {

                direction = TRUE;
            }

        }

        

}

// Funzione che si occupa di resettare i coccodrilli
void crocodile_reset(Obj *crocodile, int *reset, int value) {
    // Gestione reset coccodrillo quando il semaforo è attivo
    if (value > 0) {
        // Esegue il reset solo se non è già stato fatto
        if (*reset == 0) {
            // Elimina tutti i proiettili attivi controllando e decrementando il semaforo
            int bullet_sem_value;
            sem_getvalue(&kill_bullet_sem, &bullet_sem_value);
            
            while (bullet_sem_value > 0) {
                sem_trywait(&kill_bullet_sem);
                sem_getvalue(&kill_bullet_sem, &bullet_sem_value);
            }
            
            // Resetta posizione in base all'id del coccodrillo
            if (crocodile->id % 2 != 0) {
                // Primo coccodrillo - prima metà dello schermo
                crocodile->coordinate_obj[1] = rand() % 25;
            } else {
                // Secondo coccodrillo - seconda metà dello schermo
                crocodile->coordinate_obj[1] = rand() % (WIDTH_WINDOW - 35 + 1) + 35;
            }
            
            // Aggiorna velocità e inverte direzione
            crocodile->speed_cocc = flow_speed[(crocodile->id-1)/2];
            crocodile->direction_cocc = !crocodile->direction_cocc;
            *reset = 1; // Imposta il flag di reset attivato
            
            sem_wait(&reset_sem); // Decrementa il semaforo di reset dopo averlo gestito
        }
    } else {
        *reset = 0; // Reset del flag quando il semaforo non è attivo
        sem_init(&kill_bullet_sem, 0, 1); // Reinizializza il semaforo per i proiettili per la prossima manche
    }
}
