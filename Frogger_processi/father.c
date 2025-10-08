#include "father.h"
#include "macro_struct.h"
#include "frog.h"
#include "granade.h"
#include "crocodile.h"
#include "bullet.h"
#include "procedure_generali.h"




// Funzione che si occupa di gestire : grafica, collisioni e terminazione processi
void father_process(WINDOW **fixed_win, WINDOW **info, int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW], char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW], int p_childs_to_father[2], int p_father_to_frog[2], int p_father_to_croc[NUM_COCCODRILLI][2], int pid_frog, int pids_croc[NUM_COCCODRILLI], Obj obj, int fine_partita[2], Mode mode){

    // Inizializza le pipe che mi serviranno nel processo padre
    initialize_pipes(p_childs_to_father, p_father_to_frog, p_father_to_croc);

    // Variabili rana e granata
    int y = FROG_Y; // Coordinata y di spawn
    int x = FROG_X; // Coordinata x di spawn 

    int id_granata = 0; 
    Obj granate[MAX_GRANATE]; // Stato di tutte le granate attive
    fill_granades(granate); // Inizializziamo i campi delle granate
    


    // Variabili coccodrilli e proiettili
    Obj crocodiles[NUM_COCCODRILLI]; // Stato dei i coccodrilli attivi  
    Obj bullet[NUM_COCCODRILLI]; // Stato tutte le granate attive 
    fill_crocodile(crocodiles, bullet); // Inizializziamo i campi dei coccodrilli e dei proiettili
    int id_bullet = 0;
    


    // Variabile che uso per le diverse velocita dei flussi
    int random_number;
    
    
    // Variabili tempo
    bool game_running = true;        // Indica se il gioco è in esecuzione
    time_t start_time = time(NULL);  // Tempo di inizio della partita
    time_t ultimo_aggiornamento = time(NULL);  // Ultimo aggiornamento del timer
    


    // Tane
    int x_inizio[5] = {0}; 
    int x_fine[5] = {0};
    fill_hole(x_inizio, x_fine, NUM_HOLE); // Andiamo a salvarci le posizioni  iniziali e finali di ogni tana 


    // Vettore che salva la tana che e stata visitata
    bool tane_visitate[NUM_HOLE] = {FALSE};
    bool rana_in_tana = FALSE; // Di default setto a false la variabile che indica se la rana si trova in una tana
    

    
    bool frog_on_crocodile = FALSE; // Indica se la rana è sopra un coccodrillo
    bool isFirstTimeOnCroc = TRUE;  // Indica se la rana è sopra un coccodrillo per la prima volta
    int crocodile_index = -1;       // Indice del coccodrillo su cui si trova la rana
    

    // Offset frog_on_crocodile
    int posX = 0;
    int posY = 0;


    int id = 0; // Variabile che contiene l id del coccodrillo in cui e salita la rana
    int tempo_rimanente=0;
    Obj frog;
    bool game_paused=FALSE;
    

    while (game_running){
        
  
        chance_current_time(start_time, &ultimo_aggiornamento, *info, game_paused);  // Aggiorna il timer del turno

        rana_in_tana = FALSE;      // Setto di default la posizione della rana fuori da qualsiasi tana
        frog_on_crocodile = FALSE; // Setto di default la posizione della rana fuori da un coccodrillo


        // Legge le informazioni dei personaggi tramite p_childs_to_father
        while (read(p_childs_to_father[0], &obj, sizeof(Obj)) > 0){

           
            // Rana
            if (obj.tipo == RANA){

                // Aggiorniamo il campo della pausa 
                game_paused=obj.game_paused;
                
                manage_pause(game_paused, obj, granate, bullet, crocodiles); // Gestiamo la pausa del gioco (processi e musica)


                // Controllo se la rana si trova all altezza delle tane e gestisco le collisioni
                if (obj.coordinate_obj[0] <= HOLE_HEIGHT){

                    manage_entrance_hole(&obj, *fixed_win, *info, x_inizio, x_fine, tane_visitate, &rana_in_tana);

                    // Se la rana ha visitato tutte le 5 tane allora termina la partita
                    if (obj.tane_visitate == NUM_HOLE){
                        
                        game_running = FALSE;

                        if(obj.vite>0){
                            obj.score+=(obj.vite*50);
                        }
                    
                    }
                }
                

                if (tempo == 0 && obj.vite > 0) {

                    frog_reset(*info, rana_in_tana,&obj, p_father_to_frog);
                    crocodile_reset(p_father_to_croc, mode);
                    tempo = TEMPO;  // Resetta il tempo solo dopo aver fatto entrambi i reset
                }

                // Se la rana termina le vite, il gioco termina
                if (obj.vite <= 0){

                    game_running = FALSE;
                }
                
                
                
                // Collisione tra rana e proiettile
                check_frog_bullet_collision(&obj, bullet, NUM_COCCODRILLI, &tempo);
                


                // Collisione tra rana  e coccodrillo
                check_frog_on_crocodile(&obj,crocodiles,&frog_on_crocodile,&crocodile_index,&tempo);
                
            
                // Se il gioco non e in pausa controlliamo se la rana si trova sul coccodrillo
                if (game_paused == FALSE){

                    // Nel caso di collisione tra rana e coccodrillo gestiamo il movimento della stessa sopra il coccodrillo
                    if (frog_on_crocodile){


                        if (obj.coordinate_obj[1] >= crocodiles[crocodile_index].coordinate_obj[1] &&
                            obj.coordinate_obj[1] <= (crocodiles[crocodile_index].coordinate_obj[1] + (WIDTH_CROC - 1))){

                            // Nel caso non ci fosse collisione tra rana e coccodrillo cancelliamo la posizione della rana
                            delete_prev_frog(*fixed_win, sfondo_colori, sfondo_carattere, x, y);
                        }

                        // Disegno il coccodrillo in caso di collisione per evitare eventuali cancellazioni
                        draw_coccodrille(*fixed_win, crocodiles[crocodile_index].coordinate_obj[0], crocodiles[crocodile_index].coordinate_obj[1], crocodiles[crocodile_index].direction_cocc);
                        
                        
                        // Se la rana si trova in un coccodrillo controlliamo se si e spostata in un altro  o e rimasta nello stesso
                        if (id != crocodiles[crocodile_index].id){
                            // Nel caso abbia cambiato coccodrillo settiamo la variabile
                            isFirstTimeOnCroc = TRUE;
                        }

                        if (isFirstTimeOnCroc == TRUE){

                            is_first_time_on_crocodile(&obj,crocodiles,&crocodile_index,&isFirstTimeOnCroc,&id,&posX,&posY);
                            
                        }else{
                            
                            frog_already_on_croc(&obj,crocodiles,crocodile_index,&posX,&posY);
                            
                        }

                        // Aggiorna la posizione in base al coccodrillo
                        // Applica la correzione della coordinata x prima di aggiornare la rana

                        obj.coordinate_obj[1] = crocodiles[crocodile_index].coordinate_obj[1] + posX;
                        obj.coordinate_obj[0] = posY;

                        

                        // Setto questa varibile a false cosi da non far iniziare una nuova manche
                        obj.new_manche = FALSE;
                        
                        write(p_father_to_frog[1], &obj, sizeof(Obj));
                        
                        
                    }else{

                        // Cancella la rana dalla posizione precedente
                        delete_prev_frog(*fixed_win, sfondo_colori, sfondo_carattere, x, y);

                        frog_out_croc(&isFirstTimeOnCroc, &obj, &tempo,crocodiles);
    
                    }
                
                }

                
                
                // Aggiorna e disegna la rana nella nuova posizione
                draw_frog(*fixed_win, obj.coordinate_obj[0], obj.coordinate_obj[1]);
                y = obj.coordinate_obj[0];
                x = obj.coordinate_obj[1];
                

                
                

                // Alla fine della partita qua avremo le vite e gli score finali
                fine_partita[0] = obj.tane_visitate;
                fine_partita[1] = obj.score;

                wattron(*info, COLOR_PAIR(RED_BLACK));
                mvwprintw(*info, DATI_INFO_Y, SCORE_INFO_X, "HI-SCORE:%4d", obj.score);
                wattroff(*info, COLOR_PAIR(RED_BLACK));



			    wrefresh(*fixed_win); // Aggiorna la finestra
                


                
            }

            // Granate
            else if (obj.tipo == GRANATA){

                id_granata = obj.id;

                // Variabili controllo collisione granata sinistra e destra
                bool collision_left = FALSE;
                bool collision_right = FALSE;


                // Verifico che l'id sia valido

                if (id_granata >= 0 && id_granata < MAX_GRANATE){

                    // Controllo se ce stata la collisione tra la granata della rana e il coccodrillo
                    collision_check(*fixed_win,&obj, crocodiles,&collision_left,&collision_right); 

                    // Se non c'è stata collisione per la parte sinistra, cancella la vecchia cella in base all'area (fiume o prato)
                    if (!collision_left){

                        delete_before_granade(*fixed_win,granate[id_granata].coordinate_granata[1],granate[id_granata].coordinate_granata[0]);

                    }

                    // Analogamente, per la parte destra, se non c'è collisione, cancella la cella corrispondente
                    if (!collision_right){

                        delete_before_granade(*fixed_win,granate[id_granata].coordinate_granata[2],granate[id_granata].coordinate_granata[0]);
            
                    }

                    // Aggiorna lo stato della granata
                    granate[id_granata] = obj;

                    // Ora disegna le nuove posizioni della granata 

                    // Disegno granata sinistra
                    draw_granade_left(*fixed_win,collision_left,obj.coordinate_granata[1],obj.coordinate_granata[0],obj.granade);

                    // Disegno granata destra
                    draw_granade_right(*fixed_win,collision_right,obj.coordinate_granata[2],obj.coordinate_granata[0],obj.granade);

                
                }
            }

            // Coccodrillo
            else if (obj.tipo == COCCODRILLO){

                // Salvo l id del coccodrillo corrente(obj.id - 1 perche l array dove salviamo i coccodrilli parte da indice = 0)
                int id_crocodile = obj.id - 1;

                // Se il coccodrillo si trova in una posizione valida ,cancello la posizione precedente
                if (crocodiles[id_crocodile].coordinate_obj[0] != 0 || crocodiles[id_crocodile].coordinate_obj[1] != 0){

                    delete_prev_croc(*fixed_win, crocodiles[id_crocodile].coordinate_obj[1], crocodiles[id_crocodile].coordinate_obj[0]);
                }

                // Salvo lo stato aggiornato del coccodrillo nel vettore
                crocodiles[id_crocodile] = obj;

                // Disegno il coccodrillo nella nuova posizione
                draw_coccodrille(*fixed_win, obj.coordinate_obj[0], obj.coordinate_obj[1], obj.direction_cocc);

            }

            
            // PROIETTILI
            else if (obj.tipo == PROIETTILE){

                // Salvo l id dei proiettili (analogamente ai coccodrilli)
                id_bullet = obj.id - 1;
                bool collision_bullet = FALSE;


                collision_bullet_to_croc(bullet, id_bullet, crocodiles, &collision_bullet, fixed_win);
                

                // Aggiorna lo stato del proiettile
                bullet[id_bullet] = obj;

                // Disegna il proiettile nella nuova posizione 
                if (!collision_bullet){

                    drawing(*fixed_win, bullet[id_bullet].coordinate_obj[0], bullet[id_bullet].coordinate_obj[1], bullet[id_bullet].granade,YELLOW_BLUE,FALSE);

                }else{

                    drawing(*fixed_win, bullet[id_bullet].coordinate_obj[0], bullet[id_bullet].coordinate_obj[1], bullet[id_bullet].granade,YELLOW_BLACK,FALSE);

                }

                

            

                // controlliamo la collisione tra proiettile e granata
                for (int i = 0; i <= id_granata; i++){

                    /* Se ce una collisione tra granata e proiettile effettuo un
                    controllo della collisione con un margine di tolleranza di 1 carattere 
                    in questo modo riesco a rilevare la collisione anche se gli oggetti non sono esattamente sovrapposti 
                    questo rende la collisione più fluida*/

                    if (
                        (abs(obj.coordinate_obj[0] - granate[i].coordinate_granata[0]) <= 1) &&
                        ((abs(obj.coordinate_obj[1] - granate[i].coordinate_granata[1]) <= 1) ||
                        (abs(obj.coordinate_obj[1] - granate[i].coordinate_granata[2]) <= 1))){

                        // Cancella graficamente la granata
                        drawing(*fixed_win, granate[i].coordinate_granata[0], granate[i].coordinate_granata[1], ' ',BLUE_BLUE,FALSE);
                        drawing(*fixed_win, granate[i].coordinate_granata[0], granate[i].coordinate_granata[2], ' ',BLUE_BLUE,FALSE);


                        // Cancella graficamente il proiettile
                        drawing(*fixed_win, bullet[id_bullet].coordinate_obj[0], bullet[id_bullet].coordinate_obj[1], ' ',BLUE_BLUE,FALSE);


                        // Reset della posizione delle granate e dei proiettili
                        granate[i].coordinate_granata[0] = -10;
                        granate[i].coordinate_granata[1] = -10;
                        granate[i].coordinate_granata[2] = -10;
                        granate[i].granade = ' ';
                
                        bullet[id_bullet].coordinate_obj[0] = -10;
                        bullet[id_bullet].coordinate_obj[1] = -10;
                        bullet[id_bullet].granade = ' ';

                        
                        kill(granate[i].process_pid, SIGKILL);        // Ferma la granata
                        kill(bullet[id_bullet].process_pid, SIGKILL); // Ferma il proiettile
                    }
                }
                
                

            }

        
            
            wrefresh(*info);      // Se hai altre finestre, marca anche quelle
    

        }

        
        // Frequenza aggiornamento del padre
        usleep(5000);

        
    }


    // Termina il processo figli
    terminate_children(pid_frog,pids_croc,p_childs_to_father,p_father_to_frog,p_father_to_croc);
    
}

// Funzione che si occupa di inizializzare le pipe 
void initialize_pipes(int p_childs_to_father[2], int p_father_to_frog[2], int p_father_to_croc[NUM_COCCODRILLI][2]) {

    // Chiudi le pipe inutilizzate
    close(p_childs_to_father[1]); // lato scrittura che non serve 
    close(p_father_to_frog[0]); // lato lettura che non serve

    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        close(p_father_to_croc[i][0]); // lato lettura che non serve
    }

    // Imposta la pipe come non bloccante
    int flags = fcntl(p_childs_to_father[0], F_GETFL, 0);
    fcntl(p_childs_to_father[0], F_SETFL, flags | O_NONBLOCK);
}

// Funzione che popola il vettore di inizio e di fine delle tane
void fill_hole(int x_inizio[], int x_fine[], int num_tane){
    int pos_x_inizio = HOLE_POSITION_X; // Coordinata X iniziale della prima tana
    int pos_x_fine = 10;                // Coordinata X finale della prima tana

    // Popolo il vettore che conterra' le x di inizio di ogni tana 
    for (int i = 0; i < num_tane; i++){
        x_inizio[i] = pos_x_inizio;  // Salva la posizione iniziale della tana corrente
        pos_x_inizio += 12;          // Incrementa la posizione per la tana successiva
    }

    // Popolo il vettore che conterra' le x di fine di ogni tana 
    for (int i = 0; i < num_tane; i++){
        x_fine[i] = pos_x_fine;  // Salva la posizione finale della tana corrente
        pos_x_fine += 12;        // Incrementa la posizione per la tana successiva
    }
}

// Funzione che si occupa di gestire l' entrata o meno in tana 
void manage_entrance_hole(Obj *obj, WINDOW *fixed_win, WINDOW *info,int x_inizio[NUM_HOLE], int x_fine[NUM_HOLE], bool tane_visitate[NUM_HOLE],bool*rana_in_tana) {


    int tana_corrente = -1;  // Indice della tana in cui si trova la rana
    int tanaCenter = 0;      // Coordinata X centrale della tana corrente

    // Verifica per ogni tana se la coordinata x della rana è compresa tra x_inizio e x_fine
    for (int i = 0; i < NUM_HOLE; i++) {
        if (obj->coordinate_obj[1] >= x_inizio[i] && (obj->coordinate_obj[1] + 2) <= x_fine[i]) {

            *rana_in_tana = TRUE;
            tana_corrente = i;  // Salva l'indice della tana corrente
            tanaCenter = x_inizio[i] + (WIDTH_HOLE / 2); // Calcola il centro della tana
            obj->tane_visitate++; // Incrementa il contatore delle tane visitate
        }
    }

    // Se la rana è in una tana
    if (*rana_in_tana == TRUE) {
        system("aplay sounds/sound-frogger-coin-in.wav > /dev/null 2>&1 &"); 

        // Controlla se la tana non è già stata visitata
        if (tane_visitate[tana_corrente] == FALSE) {

            tane_visitate[tana_corrente] = TRUE;  // Segna la tana come visitata
            obj->score += 20 + tempo;  // Aggiunge punti al punteggio totale
        } 

        // Se la tana è già stata visitata, la rana perde una vita e punti
        else {
            obj->vite--;  
            if (obj->score >= 0) {
                obj->score -= 10;
            }
        }

        
        drawing(fixed_win, INIZIO_HOLE, tanaCenter, 'X', RED_BLUE, FALSE);

        // Resetta il timer per indicare il completamento del round
        tempo = 0;
    } 

    // Se la rana non è entrata in nessuna tana, resetta il timer
    else {
        tempo = 0;
    }
    
  
}

// Funzione che gestisce il timer di ogni manche
void chance_current_time(time_t start_time, time_t *ultimo_aggiornamento,WINDOW *info, bool game_paused) {

    time_t live_time = time(NULL); // Ottiene il tempo attuale

    // Se il gioco non è in pausa
    if (game_paused == FALSE) {

        // Controlla se è passato almeno un secondo dall'ultimo aggiornamento
        if (live_time > *ultimo_aggiornamento) {

            tempo--; // Decrementa il tempo di gioco

            // Aggiorna la visualizzazione del tempo nella finestra info
            wattron(info, COLOR_PAIR(RED_BLACK));
            mvwprintw(info, DATI_INFO_Y, TIMER_INFO_X, "TIME: %02d s", tempo);
            wattroff(info, COLOR_PAIR(RED_BLACK));
            wrefresh(info);

            // Aggiorna il valore dell'ultimo aggiornamento
            *ultimo_aggiornamento = live_time;
        }
    } 
    // Se il gioco è in pausa, mostra comunque il tempo rimanente senza decrementarlo
    else {
        wattron(info, COLOR_PAIR(RED_BLACK));
        mvwprintw(info, DATI_INFO_Y, TIMER_INFO_X, "TIME: %02d s", tempo);
        wattroff(info, COLOR_PAIR(RED_BLACK));
        wrefresh(info);
    }
}

// Funzione che gestisce la pausa e la ripresa della manche
void manage_pause(bool game_paused, Obj obj, Obj granade[MAX_GRANATE], Obj bullet[NUM_COCCODRILLI], Obj crocodiles[NUM_COCCODRILLI]) {

    
    // Se il gioco non è in pausa e il giocatore preme 'q' o 'Q', metti in pausa
    if (game_paused == TRUE ) {

        stop_music(); // Ferma la musica di sottofondo

        // Riprendi le granate
        for (int i = 0; i < MAX_GRANATE; i++) {
            if (granade[i].process_pid > 0) {
                kill(granade[i].process_pid, SIGSTOP);
            }
        }

       

        // Metti in pausa i coccodrilli e i proiettili
        for (int i = 0; i < NUM_COCCODRILLI; i++) {
            if (bullet[i].process_pid > 0) {
                kill(bullet[i].process_pid, SIGSTOP);
            }
            kill(crocodiles[i].process_pid, SIGSTOP);
        }

       
    } 
    // Se il gioco è in pausa e il giocatore preme 'p' o 'P', riprendi il gioco
   else if (game_paused == FALSE && obj.ch== 'p' ) {


    resume_music(); // Riprendi la musica
    

    // Riprendi le granade
    for (int i = 0; i < MAX_GRANATE; i++) {
        if (granade[i].process_pid > 0) {
            kill(granade[i].process_pid, SIGCONT);
        }
    }

    // Riprendi i coccodrilli e i proiettili
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        if (bullet[i].process_pid > 0) {
            kill(bullet[i].process_pid, SIGCONT);
        }
        kill(crocodiles[i].process_pid, SIGCONT);
    }
}

}

// Funzione che si occupa di terminare i figli e chidere le pipes
void terminate_children(pid_t pid_frog, pid_t pids_croc[], int p_childs_to_father[2], int p_father_to_frog[2], int p_father_to_croc[][2]) {
    
    // Termina i processi figli
    kill(pid_frog, SIGKILL);
    for (int i = 0; i < NUM_COCCODRILLI; i++)
    {
        if (kill(pids_croc[i], SIGKILL) == -1)
        {
            perror("Errore durante il kill");
        }
    }
    

    // Aspetta che i figli terminino
    if (waitpid(pid_frog, NULL, 0) == -1)
    {
        perror("Errore durante il kill");
    }
    for (int i = 0; i < NUM_COCCODRILLI; i++)
    {
        if (waitpid(pids_croc[i], NULL, 0) == -1)
        {
            perror("Errore durante il kill");
        }
    }
    

    // Chiudi le pipe
    close(p_childs_to_father[0]);
    close(p_father_to_frog[1]);
    for (int i = 0; i < NUM_COCCODRILLI; i++)
    {
        close(p_father_to_croc[i][1]);
    }
}

// Funzione che si occupa di resettare la rana appena la manche termina
void frog_reset(WINDOW * info ,bool rana_in_tana, Obj *frog, int p_father_to_frog[2]){
          
    // Controlla se la rana non e' in tana 
    if (rana_in_tana == FALSE){

        frog->vite--; // Decrementa le vite 

        // Se la rana ha un punteggio maggiore di 0 
        if (frog->score > 0)
        {

            frog->score -= 10; // Vengono decrementati 10 punti dal totale
        }
    }

    // Vengono resettate le posizioni 
    frog->coordinate_obj[0] = FROG_Y; 
    frog->coordinate_obj[1] = FROG_X;
    frog->new_manche = TRUE; // indichiamo che ci sara' una nuova manche (servira' nel processo frog per uccidere i processi granata)

    write(p_father_to_frog[1], frog, sizeof(Obj)); // Scriviamo la nuova frog al processo figlio

    wattron(info, COLOR_PAIR(RED_BLACK));
    mvwprintw(info, DATI_INFO_Y, LIVES_INFO_X, "LIVES:%d", frog->vite);
    wattroff(info, COLOR_PAIR(RED_BLACK));
                
            
}

// Funzione che si occupa di resettare tutti i coccodrilli appena la manche termina
void crocodile_reset(int p_father_to_croc[NUM_COCCODRILLI][2], Mode mode ){

    ResetStruct crocodile_reset;
    crocodile_reset.reset_flag = TRUE; // Imposta il flag di reset a TRUE

    int random_number=0;

    // Genera le velocità per tutti i flussi di coccodrilli
    for (int i = 0; i < VELOCITA_FLUSSI; i++)
    {
        // Genera un numero casuale tra speed_crocodile_min e speed_crocodile_max
        random_number = mode.speed_crocodile_min + rand() % (mode.speed_crocodile_max - mode.speed_crocodile_min + 1);
        crocodile_reset.velocita[i] = random_number; // Imposta velocità 
    }

    // Manda il comando di reset e la velocità individuale a ogni coccodrillo
    for (int i = 0; i < NUM_COCCODRILLI; i++){

        // Scrive sulla pipe il flag di reset e le velocità per il coccodrillo corrispondente
        if (write(p_father_to_croc[i][1], &crocodile_reset, sizeof(ResetStruct)) == -1){
            perror("Errore nella scrittura del comando di reset");
        }
    }

}

