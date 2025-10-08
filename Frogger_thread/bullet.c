#include "bullet.h"
#include "procedure_generali.h"
#include "macro_struct.h"


//Funzione che gestisce il thread per i proiettili
void *bullet_thread(void*arg){

    // Cast del puntatore all'argomento al tipo corretto
    B_Args *args = (B_Args *)arg;
    Obj *bullet = args->bullet;

    // Salvataggio dell'id del thread 
    bullet->thread_id= pthread_self();

    // Protegge l'accesso alle coordinate con mutex
    pthread_mutex_lock(&coordinate_mutex);

    // Spawn coordinata x della granata in base al verso del coccodrillo
    if (bullet->direction_cocc == FALSE){

        // DX
        bullet->coordinate_obj[1] += WIDTH_CROC+1;
    }else{
    
        // SX
		bullet->coordinate_obj[1]-=2;
    
    }

    pthread_mutex_unlock(&coordinate_mutex);


    int bullet_x=bullet->coordinate_obj[1];
    int bullet_y=bullet->coordinate_obj[0];
    
    // Produci la posizione iniziale
    produce_position(bullet);

    // Ottieni il valore iniziale del semaforo
    int value;
    sem_getvalue(&kill_bullet_sem, &value);

    // Movimento del proiettile finché è dentro lo schermo,il gioco è attivo e non e' avvenuto il reset dal coccodrillo 
    while ( game_running && 
            bullet->coordinate_obj[1] > 1 && 
            bullet->coordinate_obj[1] <= WIDTH_WINDOW - 4 && 
            value!=0) {

        if (!game_paused) { // Controlla se il gioco è in pausa

            // Ottieni il valore  del semaforo
            sem_getvalue(&kill_bullet_sem,&value);

            // Se il valore del semaforo e' maggiore di 0 allora non e' avvenuto ancora il reset dal coccodrillo
            if(value > 0){

                pthread_mutex_lock(&coordinate_mutex);

                // Aggiornamento della posizione in base alla direzione
                if (bullet->direction_cocc == TRUE) {
                    // SX
                    bullet->coordinate_obj[1] -= 1;
                } else {
                    // DX
                    bullet->coordinate_obj[1] += 1;
                }
                pthread_mutex_unlock(&coordinate_mutex);

                // Se la posizione del proiettile è cambiata , aggiorno le coordinate 
                if (bullet_x != bullet->coordinate_obj[1] || bullet_y != bullet->coordinate_obj[0]) {
                    bullet_x=bullet->coordinate_obj[1];
                    bullet_y=bullet->coordinate_obj[0];
                    produce_position(bullet);
                }
            
            }
        
        }

        usleep(args->game_mode.speed_bullet);
    }

    // Reset dei proiettili 
    pthread_mutex_lock(&coordinate_mutex);
    bullet->coordinate_obj[0] = -1;
    bullet->coordinate_obj[1] = -1; 
    bullet->granade = ' ';  
    pthread_mutex_unlock(&coordinate_mutex);

    produce_position(bullet);


    return NULL;

}

// Funzione che inizializza gli argomenti dei proiettili
void init_bullet_args(Obj *crocodile, Mode current_mode, pthread_t *thread_id_bullet,B_Args **bullet_args, bool *flag_bullet, time_t *next_bullet_time,time_t live_time) {
    
    // Alloca memoria per il proiettile
    Obj* nuovo_proiettile = (Obj*)malloc(sizeof(Obj));
    if (nuovo_proiettile!= NULL) {
        // Inizializza il proiettile
        nuovo_proiettile->coordinate_obj[0] = crocodile->coordinate_obj[0];
        nuovo_proiettile->coordinate_obj[1] = crocodile->coordinate_obj[1];
        nuovo_proiettile->granade = 'o';
        nuovo_proiettile->tipo = PROIETTILE;
        nuovo_proiettile->direction_cocc = crocodile->direction_cocc;
        nuovo_proiettile->id=crocodile->id;
        
        // Alloca gli argomenti per il thread
        *bullet_args = (B_Args*)malloc(sizeof(B_Args));
        if (*bullet_args!= NULL) {
            (*bullet_args)->bullet = nuovo_proiettile;
            (*bullet_args)->game_mode.speed_bullet = current_mode.speed_bullet;
            
            // Crea il thread
            if (pthread_create(thread_id_bullet, NULL, bullet_thread, *bullet_args) == 0) {
                *flag_bullet=FALSE;
                pthread_detach(*thread_id_bullet); // Lascia che il thread si chiuda da solo
                
                // Aggiorna il tempo per il prossimo sparo
                int intervallo_sparo = (rand() % MIN) + current_mode.t_period_spawn_bullet;
                *next_bullet_time = live_time + intervallo_sparo;
            } else {
                free(nuovo_proiettile);
                free(*bullet_args);
            }
        } else {
            free(nuovo_proiettile);
        }
    }
}





