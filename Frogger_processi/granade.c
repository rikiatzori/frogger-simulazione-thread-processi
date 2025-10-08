#include "granade.h"
#include "macro_struct.h"
#include "procedure_generali.h"
#include "crocodile.h"

// Funzione che si occupa di gestire il processo granate
void granades_process(int p_childs_to_father[2], int frog_y, int frog_x, int id){
   
    // Chiudi i lati inutilizzati delle pipe
    close(p_childs_to_father[0]); // Chiudi il lato di lettura della p_childs_to_father

     // Inizializzazione dell'oggetto granata
    Obj granate = {
        .id = id,
        .coordinate_granata = {FROG_Y, -1, -1},// Inizializza le coordinate
        .granade = '*', // Simbolo della granata
        .tipo = GRANATA,

    };

    granate.process_pid = getpid(); // Salva il PID del processo

    
    // Assegna le posizioni iniziali delle granate 
    granate.coordinate_granata[0] = frog_y;

    // Se la posizione della granata è valida, inizializza le coordinate laterali
    if (frog_x > EDGE_WINDOW){

        granate.coordinate_granata[1] = frog_x - 1;  // Posizione a sinistra
    }

    if (frog_x < (WIDTH_WINDOW-RIGHT_LIMIT)){

        granate.coordinate_granata[2] = frog_x + 1;   // Posizione a destra
    }

    // Movimento delle granate
    while (granate.coordinate_granata[1] > EDGE_WINDOW || granate.coordinate_granata[2] < WIDTH_WINDOW - 2){

        // Se la rana non si trova attacata al bordo sinistro
        if (frog_x > EDGE_WINDOW){
            if (granate.coordinate_granata[1] > EDGE_WINDOW){
                granate.coordinate_granata[1]--; // Sposta a sinistra
            }
        }
        // Se la rana non si trova attacata al bordo destro
        if (frog_x < WIDTH_WINDOW - 4){
            if (granate.coordinate_granata[2] < WIDTH_WINDOW - 2){
                granate.coordinate_granata[2]++; // Sposta a destra
            }
        }
        // Scrive le posizioni correnti delle granate sulla pipe
        write(p_childs_to_father[1], &granate, sizeof(Obj));

        usleep(50000);
    }

    // Quando la granata esce dallo schermo, resetto le coordinate
    granate.coordinate_granata[0] = -10;
    granate.coordinate_granata[1] = -10;
    granate.coordinate_granata[2] = -10;
    granate.granade = ' '; // Nessuna granata visibile

    // Scrive lo stato finale della granata sulla pipe
    write(p_childs_to_father[1], &granate, sizeof(Obj));

    // Chiudi le pipe prima di terminare
    close(p_childs_to_father[1]);
    exit(0);
}

// Funzione che si occupa di terminare tutte le granate attive una volta che la manche termina
void kill_grenades(int *id_granata, Obj frog, int p_childs_to_father[2], int *pid_grenades){

    // Itera sui processi granata attivi
    for (int i = 0; i < *id_granata; i++){

        if (pid_grenades[i] > 0){ // Controlla che il PID sia valido

            // Invia una struttura che segnala al padre il reset della granata
            Obj granade_killed = {
                .id = i,
                .coordinate_granata = {frog.coordinate_obj[0], frog.coordinate_obj[1], frog.coordinate_obj[1]},
                .tipo = GRANATA,
                .granade = ' '
            };

            write(p_childs_to_father[1], &granade_killed, sizeof(Obj));

            // Termina il processo granata
            kill(pid_grenades[i], SIGKILL);
            waitpid(pid_grenades[i], NULL, WNOHANG);

            // Azzera l'array dei PID per il processo terminato
            pid_grenades[i] = 0;
        }
    }

    // Verifica che tutte le granate siano effettivamente terminate prima di azzerare l'ID
    int granate_attive = 0;
    for (int i = 0; i < *id_granata; i++){
        
        if (pid_grenades[i] > 0){
            granate_attive++;
        }
    }

    if (granate_attive == 0){
        *id_granata = 0; // Se nessuna granata è attiva, resetta l'ID
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

// Funzione che si occupa di cancellare la posizione precedente delle granate
void delete_before_granade(WINDOW *fixed_win, int x, int y) {

    /* In base a dove si trova la granata (prato o fiume) 
    cancello di conseguenza la posizione precedente della granata stessa 
    di un colore o di un altro*/
    if (y >= END_FIUME && y <= START_FIUME) {
        drawing(fixed_win, y, x, ' ', BLUE_BLUE,FALSE);
    } else {
        drawing(fixed_win, y, x, ' ', RED_GREEN, FALSE);
    }  

}

// Funzine che si occupa di disegnare la granata sinistra
void draw_granade_left(WINDOW *fixed_win, bool collision_left, int x, int y,char granade){

    if (collision_left){

        // Se la granata passa sopra un coccodrillo, usa il colore di sfondo del coccodrillo
        drawing(fixed_win,y,x,granade,RED_BLACK,FALSE);

    }else{

        if (y > START_FIUME){

            // Se la granata si trova al di fuori del fiume usa il colore di sfondo del prato
            drawing(fixed_win,y,x,granade,RED_GREEN,FALSE);

            // Se invece la rana si avvicina al bordo, la cancelliamo 
            if (x == EDGE_WINDOW ){
    
                drawing(fixed_win,y,x,' ',RED_GREEN,FALSE);
                
            }

        } else{

            // Se la granata è sopra il fiume, usa il colore di sfondo del fiume
            drawing(fixed_win,y,x,granade,RED_BLUE,FALSE);

            // Se invece la rana si avvicina al bordo, la cancelliamo 
            if (x == EDGE_WINDOW ){
    
                drawing(fixed_win,y,x,' ',RED_BLUE,FALSE);
                
            }
        }  
    }
}

// Funzione che si occupa di disegnare la granata destra
void draw_granade_right(WINDOW *fixed_win, bool collision_right, int x, int y,char granade){

    if (collision_right){

        // Se la granata passa sopra un coccodrillo, usa il colore di sfondo del coccodrillo
        drawing(fixed_win,y,x,granade,RED_BLACK,FALSE);

    }else{

        if (y > START_FIUME){

            // Se la granata si trova al di fuori del fiume usa il colore di sfondo del prato
            drawing(fixed_win,y,x,granade,RED_GREEN,FALSE);

            // Se invece la rana si avvicina al bordo, la cancelliamo 
            if ( x == WIDTH_WINDOW - 2){
    
                drawing(fixed_win,y,x,' ',RED_GREEN,FALSE);
                
            }

        }else{

            // Se la granata è sopra il fiume, usa il colore di sfondo del fiume
            drawing(fixed_win,y,x,granade,RED_BLUE,FALSE);

            // Se invece la rana si avvicina al bordo, la cancelliamo 
            if (x == WIDTH_WINDOW - 2 ){
    
                drawing(fixed_win,y,x,' ',RED_BLUE,FALSE);
                
            }

        }
        
    }    

}

// Funzione che si occupa di controllare se e' avvenuta la collisione  tra granata e coccodrillo
void collision_check(WINDOW * fixed_win,Obj*granade, Obj crocodiles[NUM_COCCODRILLI], bool *collision_left, bool *collision_right) {
    
    // Controllo collisione per la parte sinistra della granata (coordinate_granata[1])
    for (int i = 0; i < NUM_COCCODRILLI; i++){
        
        if (granade->coordinate_granata[0] == crocodiles[i].coordinate_obj[0] &&
            granade->coordinate_granata[1] >= crocodiles[i].coordinate_obj[1] &&
            granade->coordinate_granata[1] < crocodiles[i].coordinate_obj[1] + WIDTH_CROC - 1){

            *collision_left = true;

            draw_coccodrille(fixed_win, crocodiles[i].coordinate_obj[0], crocodiles[i].coordinate_obj[1], crocodiles[i].direction_cocc);
        }
    }


    // Controllo collisione per la parte destra della granata (coordinate_granata[2])
    for (int i = 0; i < NUM_COCCODRILLI; i++){
        
        if (granade->coordinate_granata[0] == crocodiles[i].coordinate_obj[0] &&
            granade->coordinate_granata[2] >= crocodiles[i].coordinate_obj[1]+1 &&
            granade->coordinate_granata[2] < crocodiles[i].coordinate_obj[1] + WIDTH_CROC - 1){

            *collision_right = true;

            draw_coccodrille(fixed_win, crocodiles[i].coordinate_obj[0], crocodiles[i].coordinate_obj[1], crocodiles[i].direction_cocc);
        }
    }
}
