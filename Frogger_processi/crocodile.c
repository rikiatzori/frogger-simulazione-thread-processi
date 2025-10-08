#include "crocodile.h"
#include "macro_struct.h"
#include "bullet.h"
#include "procedure_generali.h"

// Funzione che disegna il coccodrillo
void draw_coccodrille(WINDOW *fixed_win, int starty, int startx, bool verso_flusso) {

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
                    drawing(fixed_win,starty+i,startx+j,crocodile[i][j], RED_BLACK, TRUE);
                }else { 
                    // Verso speculare (da destra a sinistra) , disegno la coda
                    drawing(fixed_win, starty+i, startx+j ,'|', GREEN_BLACK, TRUE);

                }
                    
            }

            // Carattere occhi del coccodrillo 
            else if (j == 1) {

                if (verso_flusso ) { 
                   // Verso normale (da sinistra a destra) disegno gli occhi
                    drawing(fixed_win, starty+i, startx+j ,crocodile[i][j], WHITE_BLACK, TRUE);

                }else { 

                    // Verso speculare (da destra a sinistra) disegno il corpo
                    drawing(fixed_win, starty+i, startx+j ,'=', GREEN_BLACK, TRUE);
                }

            }else { // Corpo del coccodrillo 

                if (verso_flusso ) { 
                    // Verso normale (da sinistra a destra) disegno il corpo
                    drawing(fixed_win, starty+i, startx+j ,crocodile[i][j], GREEN_BLACK, TRUE);

                }else { 
                		 // Verso speculare (da destra a sinistra) disegno il corpo
                    if (j > 0 && j < WIDTH_CROC-2) { 

                        drawing(fixed_win, starty+i, startx+j ,'=', GREEN_BLACK, TRUE);
                        
                    }

                    if (j == WIDTH_CROC-2) { // Disegno l occhio
                       
                        drawing(fixed_win, starty+i, startx+j ,'0', WHITE_BLACK, TRUE);
                    }

                    if (j == WIDTH_CROC-1) { // Disegno la testa speculare
                       
                        drawing(fixed_win, starty+i, startx+j ,'>', RED_BLACK, TRUE);
                    }
                }
            }
        }

    }
    
}

// Funzione che cancella la posizione precedente del coccodrillo
void delete_prev_croc(WINDOW *fixed_win, int x, int y) {

    // Cancella l'area occupata dal coccodrillo dalla posizione precedente
    for (int i = 0; i < HEIGHT_CROC; i++) {
        for (int j = 0; j < WIDTH_CROC; j++) {
		  
			
		int current_x = x + j ;
		int current_y = y + i;


		// Ripristina il colore nero ai bordi
		if (current_x <= 0 || current_x == WIDTH_WINDOW - 1) {
		 

		 drawing(fixed_win, current_y, current_x ,' ', COLOR_PAIR(COLOR_BLACK), FALSE);


		 // Disegnamo i bordi verticali,spine
		 wattron(fixed_win,COLOR_PAIR(RED_BLACK));

		 for (int y = 0; y < HEIGHT_WINDOW ; y++) {
			mvwaddch(fixed_win, y, 0, '>');  // Bordo sinistro con spine 
			mvwaddch(fixed_win, y, WIDTH_WINDOW - 1, '<');  // Bordo destro con spine 
		 }
		 
		 wattroff(fixed_win,COLOR_PAIR(RED_BLACK));
		 
		 
		} else {	// Ripristina il colore del fiume se il coccodrillo non è arrivato ai bordi
		 
		 drawing(fixed_win, current_y, current_x ,' ', BLUE_BLUE, FALSE);
		 
		}
		}
    }
}

// Funzione che gestisce il processo coccodrillo
void crocodile_process(int p_childs_to_father[2], int p_father_to_croc[NUM_COCCODRILLI][2], int cocc_x, int cocc_y, bool direction, int id, int speed, Mode mode){

    // Inizializza il coccodrillo
    Obj crocodile = {
        .id = id,
        .coordinate_obj = {cocc_y, cocc_x},
        .direction_cocc = direction,
        .speed_cocc = speed,
        .tipo = COCCODRILLO
    };

    // Salviamo il pid del coccodrillo cosi da poterlo uccidere/bloccare nel processo padre
    crocodile.process_pid = getpid();
    
    // Chiudo le estremità non utilizzate delle pipe
    close(p_childs_to_father[0]);
    close(p_father_to_croc[crocodile.id-1][1]);

    // Imposto la pipe non bloccante per la lettura
    int flags = fcntl(p_father_to_croc[crocodile.id-1][0], F_GETFL, 0);
    fcntl(p_father_to_croc[crocodile.id-1][0], F_SETFL, flags | O_NONBLOCK);
  

    // Variabili e funzioni per le velocita dei vari flussi
    int velocita[VELOCITA_FLUSSI];
    ResetStruct reset_crocodile;
    int id_temp = crocodile.id - 1;

    // Variabili e funzioni per la gestione dei proiettili di ciascun coccodrillo
    pid_t pid_bullet = 0;

    // In base alla modalità scelta l intervallo di sparo sarà piu o meno frequente 
    int intervallo_sparo = (rand() % MIN) + mode.t_period_spawn_bullet;
    // Indica il tempo in cui il prossimo proiettile dovrà essere sparato
    time_t next_bullet_time = time(NULL) + intervallo_sparo;

    // varibile che indica se il proiettile sparato ha terminato la sua corsa
    bool flag_bullet = FALSE;

    while (1){

        time_t live_time = time(NULL);

        // controllo se il processo proiettile è terminato
        if (flag_bullet ){

            pid_t result = waitpid(pid_bullet, NULL, WNOHANG);

            if (result > 0){

                // Il proiettile è terminato: resetto il flag
                flag_bullet = FALSE;

            }
        }

        // Gestione del reset e aggiornamento di posizione, velocità e direzione alla fine della manche
        if (read(p_father_to_croc[id_temp][0], &reset_crocodile, sizeof(ResetStruct)) > 0){

            // se il flag di reset e true , resettiamo i coccodrilli, tempo==0
            if (reset_crocodile.reset_flag ){

                kill_bullet(p_childs_to_father, crocodile, &pid_bullet, &flag_bullet, &next_bullet_time);

                // Esegui il reset della posizione
                if (crocodile.id % 2 != 0){

                    // Se sono il primo coccodrillo del flusso resetto la posizione nella prima meta dello schermo
                    crocodile.coordinate_obj[1] = rand() % 25;

                }else{

                    // Se sono il secondo coccodrillo del flusso resetto la posizione nella seconda meta dello schermo
                    crocodile.coordinate_obj[1] = rand() % (WIDTH_WINDOW - 35 + 1) + 35;

                }

                // Aggiorna la velocità usando l'array contenuto nella struttura
                crocodile.speed_cocc = reset_crocodile.velocita[((crocodile.id - 1) / 2)];
                // Inverte la direzione
                crocodile.direction_cocc = !crocodile.direction_cocc;
            }
        }

        // Se è trascorso il tempo per sparare e non c'è già un proiettile attivo
        if (live_time >= next_bullet_time && !flag_bullet){

            // Creo il processo coccodrillo
            pid_bullet = fork();

            if (pid_bullet < 0){

                printw("pid error");
                exit(1);

            }else if (pid_bullet == 0){

                // Processo figlio: esegue la funzione proiettile
                bullet_process(p_childs_to_father, crocodile.coordinate_obj[0], crocodile.coordinate_obj[1], crocodile.id, crocodile.direction_cocc, mode.speed_bullet);
            
            }else{

                // Processo padre: setta il flag per indicare il bullet e stato sparato
                flag_bullet = true;
            }

            //ricalcoliamo l intervallo di sparo corrente
            intervallo_sparo =  (rand() % MIN) + mode.t_period_spawn_bullet;
            next_bullet_time = live_time + intervallo_sparo;
        }

        // Movimento del coccodrillo

        // Sinistra
        if (crocodile.direction_cocc ){

            // Movimento coccodrillo
            crocodile.coordinate_obj[1] -= 1;

            // Se il coccodrillo arriva ai bordi, resettiamo le posizioni
            if (crocodile.coordinate_obj[1] <= -WIDTH_CROC){
                crocodile.coordinate_obj[1] = WIDTH_WINDOW + 10;
            }

        }else{   // Destra

            // Movimento coccodrillo
            crocodile.coordinate_obj[1] += 1;

            // Se il coccodrillo arriva ai bordi resettiamo le posizioni
            if (crocodile.coordinate_obj[1] >= WIDTH_WINDOW + WIDTH_CROC){
                crocodile.coordinate_obj[1] = -10;
            }
        }

        // Invia la posizione aggiornata al processo padre tramite pipe
        write(p_childs_to_father[1], &crocodile, sizeof(Obj));

        // Velocita del coccodrillo in base alla modalita
        usleep(crocodile.speed_cocc);
    }

    // Chiudo tutte le pipe dei coccodrilli
    close(p_father_to_croc[crocodile.id-1][0]);
    
    exit(0);
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
void crocodile_creation(int p_childs_to_father[2], int p_father_to_croc[NUM_COCCODRILLI][2], int pids_croc[NUM_COCCODRILLI],  Mode mode){

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
        for (int i = 0; i < NUM_FLUSSI; i++) {

            // Aggiorniamo la direzione
            before_direction = direction;
            
            // Generiamo la velocita in base alla modalita selezionata
            speed_generated = mode.speed_crocodile_min + rand() % ( mode.speed_crocodile_max -	mode.speed_crocodile_min + 1 );
                
            // Aggiorniamo la velocita dei flussi
            speed = speed_generated;  

            // per ogni coccodrillo della coppia
            for(int j= 0; j < 2; j++){

                // Creo il processo
                pid_t pid_croc = fork();

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



                if (pid_croc < 0) {
                    perror("Fork err");
                    exit(2);

                }

                if(pid_croc==0){

                    // Sono il figlio
                    // Funzione coccodrillo
                    crocodile_process(p_childs_to_father, p_father_to_croc, cocc_x, cocc_y, direction,cocc_id, speed, mode);
            
                }else{   //padre
                    
                    // Memorizzo tutti i pid dei coccodrilli in un array
                    pids_croc[cocc_id-1] =pid_croc;
            
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

// Funzione che controlla se il bullet passa sopra il coccodrillo
void collision_bullet_to_croc(Obj *bullet, int id_bullet, Obj *crocodiles, bool *collision_bullet, WINDOW** fixed_win) {
    
    
       
        // Itera attraverso tutti i coccodrilli per verificare se c'è una collisione con il proiettile
        for (int i = 0; i < NUM_COCCODRILLI; i++) {
            
            // Se la sua coordinata x è compresa tra l'inizio e la fine del coccodrillo
            if (bullet[id_bullet].coordinate_obj[0] == crocodiles[i].coordinate_obj[0] &&
                bullet[id_bullet].coordinate_obj[1] >= crocodiles[i].coordinate_obj[1] &&
                bullet[id_bullet].coordinate_obj[1] < crocodiles[i].coordinate_obj[1] + WIDTH_CROC) {

                *collision_bullet = TRUE;  // Modifica il valore di collision_bullet

                // Ridisegna il coccodrillo per ripristinare il suo disegno
                draw_coccodrille(*fixed_win, crocodiles[i].coordinate_obj[0], crocodiles[i].coordinate_obj[1], crocodiles[i].direction_cocc);
            }
        }

        if (!*collision_bullet) {
            // Se non c'è collisione, cancella la vecchia posizione del proiettile
            drawing(*fixed_win, bullet[id_bullet].coordinate_obj[0], bullet[id_bullet].coordinate_obj[1], ' ', BLUE_BLUE, FALSE);
        }
    
}



