#include "granade.h"
#include "macro_struct.h"
#include "procedure_generali.h"
#include "crocodile.h"



// Funzione per gestire la logica delle granate
void* granade_thread(void* arg) {
    // Cast dell'argomento al tipo corretto
    Obj *granata = (Obj *)arg;

    // Questo mi permette di prelevare il pid di ogni granata creata
    granata->thread_id= pthread_self();
    
    // Verifichiamo che la granata sia stata inizializzata correttamente
    if (granata == NULL) {
        fprintf(stderr, "Errore: argomenti della granata non validi\n");
        return NULL;
    }
    // Produci la posizione iniziale della granata per il rendering al consumatore
    produce_position(granata);
    
    // Controlliamo il valore iniziale del semaforo
    int value;
    sem_getvalue(&kill_granade_sem,&value);

    // Ciclo principale della granata
    // La granata invia aggiornamenti  fin quando il gioco e' in esecuzione, rientra entro i limiti dello schermo e il reset dalla frog non e' avvenuto
    while ( game_running && (granata->coordinate_granata[1] > LEFT_LIMIT || 
            granata->coordinate_granata[2] < WIDTH_WINDOW-2) && value!=0) {


        // Se non ci troviamo in pausa, processiamo la logica 
        if (!game_paused){

            // Controlliamo il nuovo valore del semaforo
            sem_getvalue(&kill_granade_sem,&value);

            // Se il valore del famoro e' maggiore di 0 allora il reset dalla rana non e' avvenuto 
            if(value>0){

                // Protegge l'accesso alle coordinate con mutex
                pthread_mutex_lock(&coordinate_mutex);

                // Granata sinistra
                if(granata->coordinate_granata[1] > LEFT_LIMIT){
                    granata->coordinate_granata[1]--;
                }

                // Granata destra
                if (granata->coordinate_granata[2] < WIDTH_WINDOW - 2){
                    granata->coordinate_granata[2] ++ ;
                }
                pthread_mutex_unlock(&coordinate_mutex);

                
                // Invia la nuova posizione al thread consumatore per il rendering
                produce_position(granata);
            }
            
        }
        
        usleep(50000);
    }
    
    // Pulizia finale: imposta coordinate fuori dallo schermo per farla scomparire
    granata->coordinate_granata[0] = -10;
    granata->coordinate_granata[1] = -10;
    granata->coordinate_granata[2] = -10;
    granata->granade = ' ';

    
    // Invia un ultimo messaggio per rimuovere la granata dallo schermo
    produce_position(granata);
    
    return NULL;
}

// Funzione per cancellare la posizione precedente delle granate 
void delete_before_granade(int x, int y) {

    // Se la granata di trova dentro il fiume cancelliamo di blu, altrimenti se si trova nel prato cancelliamo di verde
    if (y >= END_FIUME && y <= START_FIUME) {
        drawing(y, x, ' ', BLUE_BLUE,FALSE);
    } else {
        drawing( y, x, ' ', RED_GREEN, FALSE);
    }  

}

// Funzione per disegnare la granata sinistra
void draw_granade_left(bool collision_left, int x, int y,char granade){

    if (collision_left){

        //se la granata passa sopra il coccodrillo disegna di RED_BLACK
        drawing(y,x,granade,RED_BLACK,FALSE);

    }else{

        if (y > START_FIUME){

            drawing(y,x,granade,RED_GREEN,FALSE);

            // Se la granata si trova alla coordinata 1 "cambiamo il carattere" in modo da avere un effetto grafico migliore
            if (x == 1 ){
    
                drawing(y,x,' ',RED_GREEN,FALSE);
                
            }

        } else{

            drawing(y,x,granade,RED_BLUE,FALSE);

            if (x == 1 ){
    
                drawing(y,x,' ',RED_BLUE,FALSE);
                
            }
        }  
    }
}

// Funzione per disegnare la granata destra
void draw_granade_right( bool collision_right, int x, int y,char granade){

    if (collision_right){

        //se la granata passa sopra il coccodrillo disegna di RED_BLACK
        drawing(y,x,granade,RED_BLACK,FALSE);

    }else{

        if (y > START_FIUME){

            drawing(y,x,granade,RED_GREEN,FALSE);

            // Se la granata si trova alla coordinata 1 "cambiamo il carattere" in modo da avere un effetto grafico migliore
            if ( x == WIDTH_WINDOW - 2){
    
                drawing(y,x,' ',RED_GREEN,FALSE);
                
            }

        }else{

            drawing(y,x,granade,RED_BLUE,FALSE);

            if (x == WIDTH_WINDOW - 2 ){
    
                drawing(y,x,' ',RED_BLUE,FALSE);
                
            }

        }
        
    }    

}

// Funzione per verificare la collisione con i coccodrilli
void collision_check(Obj*granade, Obj crocodiles[NUM_COCCODRILLI], bool *collision_left, bool *collision_right) {
    
    // Controllo collisione per la parte sinistra della granata (coordinate_granata[1])
    for (int i = 0; i < NUM_COCCODRILLI; i++){
        
        if (granade->coordinate_granata[0] == crocodiles[i].coordinate_obj[0] &&
            granade->coordinate_granata[1] >= crocodiles[i].coordinate_obj[1] &&
            granade->coordinate_granata[1] < crocodiles[i].coordinate_obj[1] + WIDTH_CROC ){

            *collision_left = TRUE;

            draw_coccodrille( crocodiles[i].coordinate_obj[0], crocodiles[i].coordinate_obj[1], crocodiles[i].direction_cocc);
        }
    }


    // Controllo collisione per la parte destra della granata (coordinate_granata[2])
    for (int i = 0; i < NUM_COCCODRILLI; i++){
        
        if (granade->coordinate_granata[0] == crocodiles[i].coordinate_obj[0] &&
            granade->coordinate_granata[2] >= crocodiles[i].coordinate_obj[1]+1 &&
            granade->coordinate_granata[2] < crocodiles[i].coordinate_obj[1] + WIDTH_CROC ){

            *collision_right = TRUE;

            draw_coccodrille(crocodiles[i].coordinate_obj[0], crocodiles[i].coordinate_obj[1], crocodiles[i].direction_cocc);
        }
    }
}

// Funzione che popola l' array delle granate
void fill_granades(Obj granate[MAX_GRANATE]){
    
    for (int i = 0; i < MAX_GRANATE; i++){

        granate[i].coordinate_granata[0] = -1;
        granate[i].coordinate_granata[1] = -1;
        granate[i].coordinate_granata[2] = -1;
    }
}

// Funzione per inizializzare glia rgomenti delle granate
void granade_args(int* grenade_shoot_id, Obj* frog, pthread_t thread_id_granade[]){

    Obj* granade_args = malloc(sizeof(Obj));
                    
        if(granade_args != NULL){

            granade_args->coordinate_granata[0] = frog->coordinate_obj[0];  // Y della granata == Y della rana
            granade_args->coordinate_granata[1] = frog->coordinate_obj[1];  // X della granata sinistra == X della rana
            granade_args->coordinate_granata[2] = frog->coordinate_obj[1]+2;  // X della granata destra == X della rana +2 (cosi da farla spawnare "sotto" il corpo della rana)
            granade_args->granade = '*';
            granade_args->tipo= GRANATA;
            granade_args->id = *grenade_shoot_id;
            
            
            // Crea il thread per la granata
            if (pthread_create(&thread_id_granade[*grenade_shoot_id], NULL, granade_thread, granade_args) != 0) {
                fprintf(stderr, "Errore nella creazione del thread granata\n");
                free(granade_args);
            } else {
                // Thread creato con successo
                pthread_detach(thread_id_granade[*grenade_shoot_id]);
                // Incremento il valore per tenere il conto delle granate,a 10 non spawnano piu granate
                (*grenade_shoot_id)++;
                
            }

        }
}

