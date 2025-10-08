#include "consumer.h"
#include "macro_struct.h"
#include "frog.h"
#include "granade.h"
#include "crocodile.h"
#include "bullet.h"
#include "procedure_generali.h"

// Funzione che gestisce il timer della manche
void chance_current_time(time_t start_time, time_t *ultimo_aggiornamento, bool game_paused){
    time_t live_time = time(NULL);
    if (game_paused == FALSE)
    {
        if (live_time > *ultimo_aggiornamento)
        {
            tempo--; // Decrementa il tempo
            pthread_mutex_lock(&ncurses_mutex);
        
            wattron(info, COLOR_PAIR(RED_BLACK));
            mvwprintw(info, DATI_INFO_Y, TIMER_INFO_X, "TIME: %02d s", tempo);
            wattroff(info, COLOR_PAIR(RED_BLACK));
            wrefresh(info);

            pthread_mutex_unlock(&ncurses_mutex);
            *ultimo_aggiornamento = live_time; // Aggiorna il tempo
        }
    }
    else
    {
        pthread_mutex_lock(&ncurses_mutex);
        wattron(info, COLOR_PAIR(RED_BLACK));
        mvwprintw(info, DATI_INFO_Y, TIMER_INFO_X, "TIME: %02d s", tempo);
        wattroff(info, COLOR_PAIR(RED_BLACK));
        wrefresh(info);
        pthread_mutex_unlock(&ncurses_mutex);
    }
}

// Funzione che gestisce il buffer produttore-consumatore
void buffer_manager(Obj *current_obj){

    sem_wait(&shared_buffer.full); // Aspetta che ci sia qualcosa da consumare nel buffer 


    pthread_mutex_lock(&shared_buffer.mutex); // Accesso sicuro al buffer condiviso attraverso mutex
        
    // Preleva l'oggetto dal buffer circolare e aggiorna l'indice di uscita
    *current_obj = shared_buffer.buffer[shared_buffer.out];
    shared_buffer.out = (shared_buffer.out + 1) % BUFFER_SIZE;
        
        
    pthread_mutex_unlock(&shared_buffer.mutex); // Rilascia il mutex del buffer dopo aver prelevato l'oggetto
        
        
    sem_post(&shared_buffer.empty); // Segnala che c'è uno slot vuoto nel buffer per i threads produttori
 


}

// Funzione che si occupa di resettare tutti gli oggetti appena avviene una delle casistiche che porta alla terminazione dela manche
void reset_manche( Obj granade[MAX_GRANATE],Obj current_obj,bool collision_bullet_frog, bool drop_on_the_river ,bool *reset,Mode *mode) {
    // Verifica se è avvenuto il reset della manche
    if (current_obj.coordinate_obj[0] <= HOLE_HEIGHT || 
        collision_bullet_frog || 
        drop_on_the_river || 
        tempo == 0) {
            

        
        // Esegui il reset solo se non è già stato fatto in questo ciclo
        if (!*reset) {
            

            // Aggiorna le velocità dei flussi dei coccodrilli

            pthread_mutex_lock(&flow_speed_mutex);
            for (int i = 0; i < NUM_FLUSSI; i++) {
                flow_speed[i] = mode->speed_crocodile_min + rand() % (mode->speed_crocodile_max - mode->speed_crocodile_min + 1);
            }
            pthread_mutex_unlock(&flow_speed_mutex);


            // Invia segnale di reset a tutti i coccodrilli e alla rana
            for (int i = 0; i <= NUM_COCCODRILLI; i++) {
                sem_post(&reset_sem);
            }

            // Aggiorna il display del tempo con formato fisso a 2 cifre
            wattron(info, COLOR_PAIR(RED_BLACK));
            mvwprintw(info, DATI_INFO_Y, TIMER_INFO_X, "TIME: %02d s", tempo);
            wattroff(info, COLOR_PAIR(RED_BLACK));
            wrefresh(info);

            // Segna che il reset è stato eseguito in questo ciclo
            *reset = 1;

            // Elimina a livello grafico le granate terminate
            for (int i = 0; i < MAX_GRANATE; i++) {
                delete_before_granade( granade[i].coordinate_granata[1], granade[i].coordinate_granata[0]);
            }
        }
    } else {
        // Reset del flag quando la manche inizia
        *reset = 0;
        
    }
}

// Funzione consumatore che si occcuperà di gestire grafica e collisioni
void* consumer_thread(void* arg) {

    // Estrae la struttura Mode passata come argomento
    // In questo modo avremmo la modalità selezionata che useremò per velocita dei nemici ecc
    Mode *mode = (Mode *)arg;

    Obj  current_obj; // Oggetto che leggiamo dal buffer

    // Inizializza dati della rana
    int frog_x = FROG_X;
    int frog_y = FROG_Y;

    
    // Inizializza dati delle granate
    Obj granade[MAX_GRANATE];
    fill_granades(granade);
    int id_granata;
    
    // Inizializza dati coccodrilli e proiettili
    Obj bullets[NUM_COCCODRILLI];
    Obj croc[NUM_COCCODRILLI];
    int bullet_id;
    fill_crocodile(croc, bullets); // Inizializzazione degli oggetti coccodrillo e proiettile

    
    int id=0; // Id che indica il coccodrillo in cui ci trovavamo in precedenza
    int crocodile_index; // Id che indica il coccodrillo con cui abbiamo appena colliso
    int posX; // posizione che indica l offset x tra rana e coccodrillo
    int posY; // posizione che indica l offset y tra rana e coccodrillo

    bool frog_on_croc = FALSE;     // Flag per verificare se la rana è sopra un coccodrillo
    bool firs_time_on_croc= FALSE; // Flag per verificare se la rana è salita per la prima volta in quel coccodrillo 
    bool collision_bullet_frog=FALSE; // Flag per verificare la collisione tra proiettile e rana
    bool drop_on_the_river=FALSE; // Flag per verificare se la rana è caduta nel fiume
    bool reset = FALSE; // Flag per garantire che il reset della manche venga chiamato solo una volta

    // Variabili per la gestione del tempo di gioco
    time_t start_time = time(NULL);         // Memorizza il tempo di inizio partita
    time_t ultimo_aggiornamento = time(NULL); 


    while(game_running) {

        // Reset di tutti i flag 
        frog_on_croc = FALSE;
        firs_time_on_croc = FALSE;
        drop_on_the_river = FALSE;
        collision_bullet_frog=FALSE;

        
        // Aggiorna il timer di gioco 
        pthread_mutex_lock(&game_state_mutex);
        chance_current_time(start_time, &ultimo_aggiornamento, game_paused);
        pthread_mutex_unlock(&game_state_mutex);
        
        // Leggiamo dal buffer
        buffer_manager(&current_obj);
               
        // Processa l'oggetto solo se il gioco non è in pausa
        if (!game_paused) {

            pthread_mutex_lock(&ncurses_mutex); // Richiamiamo il blocco del mutex per avere l'accesso eclusivo ad ncurses 

            
            // GESTIONE RANA
            if(current_obj.tipo == RANA) {

                // Controllo la collisione tra rana e proiettile
                check_frog_bullet_collision(&current_obj,bullets,&collision_bullet_frog);

                // Controllo la collisione tra rana e coccodrillo
                check_frog_on_crocodile(&current_obj,croc,&frog_on_croc,&crocodile_index);

                // Se la rana si trova sul coccodrillo
                if(frog_on_croc){

                    // Gestione grafica
                    if (current_obj.coordinate_obj[1] >= croc[crocodile_index].coordinate_obj[1] &&
                        current_obj.coordinate_obj[1] <= (croc[crocodile_index].coordinate_obj[1] + (WIDTH_CROC - 1))){

                        // Nel caso ci fosse collisione tra rana e coccodrillo cancelliamo la posizione della rana
                        delete_prev_frog(frog_x, frog_y, current_obj.tana_visitata, current_obj.centro_tana);
                    }

                    // Disegno il coccodrillo in caso di collisione per evitare eventuali cancellazioni
                    draw_coccodrille(croc[crocodile_index].coordinate_obj[0], croc[crocodile_index].coordinate_obj[1], croc[crocodile_index].direction_cocc);
                    
      
                    // Se la rana si trova in un coccodrillo controlliamo se si e spostata in un altro  o e rimasta nello stesso
                    if (id != croc[crocodile_index].id ){

                        // Nel caso abbia cambiato coccodrillo settiamo la variabile
                        firs_time_on_croc = TRUE;
                        
                    }

                    if (firs_time_on_croc == TRUE){

                        // Salviamo le posizioni iniziali rispetto al coccodrillo
                        is_first_time_on_crocodile(&current_obj,croc,&crocodile_index,&firs_time_on_croc,&id,&posX);
                        
                    }else{
                        
                        // Permettiamo il movimento all' interno del coccodrillo
                        frog_already_on_croc(&current_obj,croc,crocodile_index,&posX);
                        
                    }

                    // Controlliamo il valore del semaforo , se risulta positivo allora dobbiamo avvisare il thread rana e aggiornare la posizione
                    int value_sem_offset;
                    sem_getvalue(&frog_on_croc_sem,&value_sem_offset);
                    
                    if(value_sem_offset > 0){

                        // Decrementiamo il contatore del semaforo
                        sem_trywait(&frog_on_croc_sem);

                        // Aggiorniamo l' offset 
                        pthread_mutex_lock(&offset_mutex);
                        crocodile_offset_x = croc[crocodile_index].coordinate_obj[1] + posX;
                        pthread_mutex_unlock(&offset_mutex);
                        
                    }
                    

                }else{
                    
                    // Azzeriamo l' id che indica il coccodrillo corrente
                    id=0;
                    
                    // Cancella la rana dalla posizione precedente
                    delete_prev_frog(frog_x, frog_y, current_obj.tana_visitata, current_obj.centro_tana);

                    // Controlliamo se ci troviamo nel fiume oppure nel prato
                    frog_out_croc(&firs_time_on_croc, &current_obj, croc,&drop_on_the_river);
    
                }

                reset_manche(granade,current_obj,collision_bullet_frog,drop_on_the_river,&reset,mode);

                // Se il tempo è uguale a 0 , significa che la manche è finita e quindi dobbiamo riportare il timer al valore di default
                if(tempo==0){

                    pthread_mutex_lock(&game_state_mutex);
                    tempo=TEMPO;
                    pthread_mutex_unlock(&game_state_mutex);
                }


                // Disegna la rana nella nuova posizione
                draw_frog(current_obj.coordinate_obj[0], current_obj.coordinate_obj[1]);

                // Aggiorna le coordinate correnti della rana per il prossimo ciclo
                frog_x = current_obj.coordinate_obj[1];
                frog_y = current_obj.coordinate_obj[0];
                
               
                // Aggiorna il punteggio visualizzato nell'interfaccia
                wattron(info, COLOR_PAIR(RED_BLACK));
                mvwprintw(info, DATI_INFO_Y, SCORE_INFO_X, "HI-SCORE:%4d", current_obj.score);
                wattroff(info, COLOR_PAIR(RED_BLACK));

                // Aggiorna le vite visualizzate nell'interfaccia
                wattron(info, COLOR_PAIR(RED_BLACK));
                mvwprintw(info, DATI_INFO_Y, LIVES_INFO_X, "LIVES:%d", current_obj.vite);
                wattroff(info, COLOR_PAIR(RED_BLACK));
                
                // Aggiorna le finestre per mostrare le modifiche
                wrefresh(fixed_win);
                wrefresh(info);

                
                end_game(current_obj);
                
            }
            
            // GESTIONE GRANATA
            else if(current_obj.tipo == GRANATA){

                // Salviamo l id della granta che stiamo leggendo
                int id_granata = current_obj.id;

                // Variabili controllo collisione granata sinistra e destra
                bool collision_left = FALSE;
                bool collision_right = FALSE;
               
                // Se la granata rientra nell intervallo corretto
                if (id_granata >= 0 && id_granata < MAX_GRANATE){

                    // Controllo se c'é stata la collisione tra la granata della rana e il coccodrillo
                    collision_check(&current_obj, croc,&collision_left,&collision_right); 

                    // Se non c'è stata collisione per la parte sinistra, cancella la vecchia cella in base all'area (fiume o prato)
                    if (!collision_left){

                        delete_before_granade(granade[id_granata].coordinate_granata[1],granade[id_granata].coordinate_granata[0]);

                    }

                    // Analogamente, per la parte destra, se non c'è collisione, cancella la cella corrispondente
                    if (!collision_right){

                        delete_before_granade(granade[id_granata].coordinate_granata[2],granade[id_granata].coordinate_granata[0]);
            
                    }

                    // Aggiorna lo stato della granata
                    granade[id_granata] = current_obj;

                    // Ora disegna le nuove posizioni della granata 

                    // Disegno granata sinistra
                    draw_granade_left(collision_left,current_obj.coordinate_granata[1],current_obj.coordinate_granata[0],current_obj.granade);

                    // Disegno granata destra
                    draw_granade_right(collision_right,current_obj.coordinate_granata[2],current_obj.coordinate_granata[0],current_obj.granade); 
                    
                }
            }

            // GESTIONE COCCODRILLI
            else if(current_obj.tipo == COCCODRILLO) {

                // Salviamo l id del coccodrillo che abbiamo letto
                int current_id = current_obj.id - 1;


                // Cancella la posizione precedente del coccodrillo se era già visibile
                if(croc[current_id].coordinate_obj[0] != 0 || croc[current_id].coordinate_obj[1] != 0) {

                    delete_prev_croc(croc[current_id].coordinate_obj[1], croc[current_id].coordinate_obj[0]);
                    
                }
                
                // Disegna il coccodrillo nella nuova posizione con la corretta direzione
                draw_coccodrille(current_obj.coordinate_obj[0], current_obj.coordinate_obj[1], current_obj.direction_cocc);
                
                
                // Aggiorna l'oggetto coccodrillo nel registro locale
                croc[current_id] = current_obj;
            }
            
            // GESTIONE PROIETTILI
            else if(current_obj.tipo == PROIETTILE) {

                // Salva l id del proiettile letto 
                bullet_id = current_obj.id - 1; 
                bool collision_bullet = FALSE;  // Flag per rilevare collisioni
                

                // Verifica collisioni tra proiettile e coccodrilli
                collision_bullet_to_croc(bullets, bullet_id, croc, &collision_bullet);

                // Aggiorna lo stato del proiettile
                bullets[bullet_id] = current_obj;

                // Disegna il proiettile con colore appropriato in base alla collisione
                if (!collision_bullet) {
                    // Disegna proiettile normale (su acqua)
                    drawing(bullets[bullet_id].coordinate_obj[0], bullets[bullet_id].coordinate_obj[1], bullets[bullet_id].granade, YELLOW_BLUE, FALSE);
                } else {
                    // Disegna proiettile in collisione (su coccodrillo)
                    drawing(bullets[bullet_id].coordinate_obj[0], bullets[bullet_id].coordinate_obj[1],bullets[bullet_id].granade, YELLOW_BLACK, FALSE);
                }

                // Verifica collisione tra proiettile e granata
                collision_bullet_granade(granade,bullets,current_obj,bullet_id);
  
            }


            // Rilascia il mutex per le operazioni di disegno
            pthread_mutex_unlock(&ncurses_mutex);

        }


    }
    
    // Termina il thread quando il gioco è finito
    return NULL;
}

// Funzione che termina la partita
void end_game(Obj current_obj){

    // Termina il gioco se la rana ha esaurito tutte le vite
    if(current_obj.vite <= 0 || current_obj.tane_visitate==NUM_HOLE) {
       

        pthread_mutex_lock(&game_state_mutex);
        game_running = FALSE;
        pthread_mutex_unlock(&game_state_mutex);
    }
}

// Funzione che verifica la collisione tra proiettile e granate
void collision_bullet_granade(Obj granade[MAX_GRANATE],Obj bullets[NUM_COCCODRILLI],Obj current_obj, int bullet_id){

    for (int i = 0; i < MAX_GRANATE; i++){

        /* Controllo della collisione con tolleranza di 1 carattere */
        if ((abs(current_obj.coordinate_obj[0] - granade[i].coordinate_granata[0]) <= 1) &&
            ((abs(current_obj.coordinate_obj[1] - granade[i].coordinate_granata[1]) <= 1) ||
             (abs(current_obj.coordinate_obj[1] - granade[i].coordinate_granata[2]) <= 1))){
          
            // Cancella graficamente la granata
            drawing(granade[i].coordinate_granata[0], granade[i].coordinate_granata[1], ' ', BLUE_BLUE, FALSE);
            drawing(granade[i].coordinate_granata[0], granade[i].coordinate_granata[2], ' ', BLUE_BLUE, FALSE);
          
            // Cancella graficamente il proiettile
            drawing(bullets[bullet_id].coordinate_obj[0], bullets[bullet_id].coordinate_obj[1], ' ', BLUE_BLUE, FALSE);
            
            // Se hai un thread per ogni granata, cancellalo
            if (granade[i].thread_id != 0) {
                pthread_cancel(granade[i].thread_id);

                pthread_join(granade[i].thread_id, NULL);
                granade[i].thread_id = 0;
            }
            
            // Se hai un thread per ogni proiettile, cancellalo
            if (bullets[bullet_id].thread_id != 0) {
                pthread_cancel(bullets[bullet_id].thread_id);

                pthread_join(bullets[bullet_id].thread_id, NULL);
                bullets[bullet_id].thread_id = 0;
            }
            
            // Reset della posizione delle granate e dei proiettili
            granade[i].coordinate_granata[0] = -10;
            granade[i].coordinate_granata[1] = -10;
            granade[i].coordinate_granata[2] = -10;
            granade[i].granade = ' ';

            bullets[bullet_id].coordinate_obj[0] = -10;
            bullets[bullet_id].coordinate_obj[1] = -10;
            bullets[bullet_id].granade = ' ';
            }
    }
   

}

// Funzione che si occupa di distruggere mutex e semafori
void mutex_semaphore_destroy() {

    pthread_mutex_destroy(&coordinate_mutex);
    pthread_mutex_destroy(&offset_mutex);
    pthread_mutex_destroy(&game_state_mutex);
    pthread_mutex_destroy(&flow_speed_mutex);
    pthread_mutex_destroy(&ncurses_mutex);
    pthread_mutex_destroy(&shared_buffer.mutex);
    
    sem_destroy(&shared_buffer.empty);
    sem_destroy(&shared_buffer.full);
    sem_destroy(&kill_bullet_sem);
    sem_destroy(&kill_granade_sem);
    sem_destroy(&reset_sem);
    sem_destroy(&frog_on_croc_sem);
}

// Funzione che si occupa di terminare i thread, liberare la memoria e salvare i dati di fine partita
void kill_threads(pthread_t thread_id_consumer, pthread_t thread_id_rana, pthread_t thread_id_crocodiles[NUM_COCCODRILLI], Mode *consumer_args, C_Args *crocodile_args[], F_Args *frog_args, int end_game[]) {
    
    // Aspetta la terminazione del thread consumatore
    pthread_join(thread_id_consumer, NULL);
    
    // Memorizza i dati finali del gioco (ad esempio, punteggio e tane visitate)
    end_game[0] = frog_args->frog->tane_visitate;
    end_game[1] = frog_args->frog->score;
    
    // Libera la memoria allocata per gli argomenti del thread consumatore
    free(consumer_args);

    // Aspetta la terminazione del thread della rana
    pthread_join(thread_id_rana, NULL);
    
    // Libera la memoria allocata per gli argomenti del thread della rana
    free(frog_args);

    // Aspetta la terminazione dei thread dei coccodrilli
    for (int i = 0; i < NUM_COCCODRILLI; i++) {
        pthread_join(thread_id_crocodiles[i], NULL);
        
        // Libera la memoria allocata per gli argomenti di ciascun thread coccodrillo
        free(crocodile_args[i]);
    }
}

