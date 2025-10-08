#include "frog.h"
#include "macro_struct.h"
#include "granade.h"
#include "procedure_generali.h"



void draw_frog(WINDOW *fixed_win, int starty, int startx) {
    
    char frog[HEIGHT_FROG][WIDTH_FROG] = {
        {'o', '^', 'o'},
        {'!', '=', '!'}
    };
 

    // Disegna la rana carattere per carattere con colori diversi
    for (int i = 0; i < HEIGHT_FROG; i++) {
        for (int j = 0; j < WIDTH_FROG; j++) {
            if (i == 0 && j == 1) { 

                // Colore per la testa (il simbolo '^')
                drawing(fixed_win,starty+i,startx+j,frog[i][j],MAGENTA_BLACK,FALSE);

            } else if(i == 0 && (j == 0 || j == 2)){
                
                // Colore per gli "occhi"
                drawing(fixed_win,starty+i,startx+j,frog[i][j],WHITE_BLACK,FALSE);

            } else if (i == 1) { 
                
                // Colore per il corpo
                drawing(fixed_win,starty+i,startx+j,frog[i][j],MAGENTA_BLACK,FALSE);

            } 
            
        }

    }

}

// Questa fuznione si occupa di salvare in una matrice la posizione e i rispettivi caratteri della rana per poi cancellarla completamente 
void delete_prev_frog(WINDOW * fixed_win,int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW],char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW],int x , int y){

    // Cancella la rana dalla posizione precedente
    for (int i = 0; i < HEIGHT_FROG; i++) {
        for (int j = 0; j < WIDTH_FROG; j++) {

            wattron(fixed_win, sfondo_colori[y + i][x + j]);
            mvwaddch(fixed_win, y + i, x + j, sfondo_carattere[y + i][x + j]);
            wattroff(fixed_win, sfondo_colori[y + i][x + j]);

        }
    }

}

void frog_process(int p_childs_to_father[2], int p_father_to_frog[2], WINDOW **fixed_win, Mode mode){

    Obj frog = {

        .vite = mode.vite_rana,
        .coordinate_obj = {FROG_Y, FROG_X},
        .score = 0,
        .id = 1,
        .tipo = RANA,
        .tane_visitate = 0,
        .game_paused=FALSE

    };

    frog.process_pid = getpid();

    close(p_childs_to_father[0]); // Chiudi il lato di lettura della pipe
    close(p_father_to_frog[1]);

    // Imposta la p_father_to_frog[0] come non bloccante
    int flags = fcntl(p_father_to_frog[0], F_GETFL, 0);
    fcntl(p_father_to_frog[0], F_SETFL, flags | O_NONBLOCK);

    pid_t pid_granade;
    pid_t pid_grenades[MAX_GRANATE];
    int id_granata = 0;

    Obj obj;

    while (1){

        // Leggi l'input dalla finestra
        frog.ch = wgetch(*fixed_win);

        // Gestisci i comandi di pausa/ripresa
        if (frog.ch == 'q' || frog.ch == 'Q'){

            frog.game_paused = TRUE;

        } else if (frog.ch == 'p' || frog.ch == 'P'){

            frog.game_paused = FALSE;

        }

        // Se non siamo in pausa, processa i comandi di movimento e azioni
        if (frog.game_paused == FALSE){

            if (frog.ch == KEY_UP && frog.coordinate_obj[0] > UPPER_LIMIT){

                frog.coordinate_obj[0] -= VERTICAL_MOV;

            }else if (frog.ch == KEY_DOWN && frog.coordinate_obj[0] < HEIGHT_WINDOW - LOWER_LIMIT){

                frog.coordinate_obj[0] += VERTICAL_MOV;

            }else if (frog.ch == KEY_LEFT && frog.coordinate_obj[1] > LEFT_LIMIT){

                if (frog.coordinate_obj[1] % 2 == 0){

                    frog.coordinate_obj[1] -= 1;

                }else {

                    frog.coordinate_obj[1] -= HORIZONTAL_MOV;

                }

            }else if (frog.ch == KEY_RIGHT && frog.coordinate_obj[1] < WIDTH_WINDOW - RIGHT_LIMIT){
                
                if (frog.coordinate_obj[1] % 2 == 0){

                    frog.coordinate_obj[1] += 1;

                }else {

                    frog.coordinate_obj[1] += HORIZONTAL_MOV;

                }

            }else if (frog.ch == ' '){
                // Se il gioco non è in pausa, la rana può sparare granate
                if (frog.coordinate_obj[0] > END_FIUME-2 && id_granata < MAX_GRANATE){
                   
                    pid_granade = fork();
                    if (pid_granade == 0){
                        // Processo figlio per la granata
                        granades_process(p_childs_to_father, frog.coordinate_obj[0], frog.coordinate_obj[1], id_granata);
                    }else if (pid_granade > 0){
                       
                        pid_grenades[id_granata] = pid_granade;
                        id_granata++; // Incrementa l'ID della granata
                   
                    } else{
                        
                        printw("pid error");
                        exit(1);

                    }
                }
            }
        
        
            
        }

        // Controlla eventuali aggiornamenti inviati dal padre
        if (read(p_father_to_frog[0], &obj, sizeof(Obj)) > 0){
            
            // Copia l'input corrente (ch) nell'oggetto aggiornato, così da mantenere coerente la logica di movimento
            obj.ch = frog.ch;
            //Aggiorniamo la frog corrente cosi da poter aggangiare la rana al coccodrillo
            frog = obj;

            if (obj.new_manche == TRUE) {

                // Termina tutte le granate esistenti
                kill_grenades(&id_granata, frog, p_childs_to_father, pid_grenades);

            }
        }

       // if(((frog.game_paused && (frog.ch == 'p' || frog.ch == 'q'))) || !frog.game_paused){

            // Invia le coordinate aggiornate al padre
            write(p_childs_to_father[1], &frog, sizeof(Obj));
            
        //}
        

        usleep(30000); // Pausa per ridurre il consumo di CPU
    }

    close(p_childs_to_father[1]);
    close(p_father_to_frog[0]);
    exit(0);
}

void frog_out_croc(bool *isFirstTimeOnCroc, Obj *obj, int *tempo,Obj crocodiles[NUM_COCCODRILLI]){

    // se la rana non si trova nel coccodrillo , allora setto questa variabile a true
    // facendo questo faccio si che se poi la rana sale su un coccodrillo , setto l offset e le posizioni iniziali
    *isFirstTimeOnCroc = TRUE;
    bool drop_on_the_river = FALSE;

    // se la rana si trova all interno del fiume
    if (obj->coordinate_obj[0] >= END_FIUME && obj->coordinate_obj[0] <= START_FIUME){

        // ciclo su tutti i coccodrilli e vedo se la rana si trova all esterno del coccodrillo
        for (int i = 0; i < NUM_COCCODRILLI; i++){

            if (obj->coordinate_obj[0] == crocodiles[i].coordinate_obj[0] && (obj->coordinate_obj[1] + 1 < (crocodiles[i].coordinate_obj[1])) || (obj->coordinate_obj[1] + 1 > (crocodiles[i].coordinate_obj[1]) + (WIDTH_CROC - 1))){

                // se la rana non si trova piu sul coccodrillo corrente, si perde la manche
                drop_on_the_river = TRUE;
                *tempo = 0;

            }
        }

        if (drop_on_the_river == TRUE){
            system("aplay sounds/water-splash.wav > /dev/null 2>&1 &");
            drop_on_the_river = FALSE;
        }
    }
                
}

// Funzione per quando un proiettile becca la rana
void check_frog_bullet_collision(Obj *frog, Obj bullet[], int num_bullets, int *tempo) {
    
    for (int i = 0; i < num_bullets; i++) {
        if (frog->coordinate_obj[0] == bullet[i].coordinate_obj[0] && (frog->coordinate_obj[1] == bullet[i].coordinate_obj[1] || (frog->coordinate_obj[1] + 2) == bullet[i].coordinate_obj[1])) {
            
            // Collisione rilevata, la rana perde la manche
            system("aplay sounds/bullet_to_frog.wav > /dev/null 2>&1 &");
            *tempo = 0;
                 
        }
    }
}

// Funzione per controllare che la rana si trovi sopra un coccodrillo
void check_frog_on_crocodile(Obj *obj, Obj crocodiles[NUM_COCCODRILLI], bool *frog_on_crocodile, int *crocodile_index, int *tempo) {
   
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
            } else {
                // Se cade in acqua allora resettiamo la manche
                *tempo = 0;
            }
        }
    }

}

void is_first_time_on_crocodile(Obj *obj, Obj *crocodiles, int *crocodile_index,bool *isFirstTimeOnCroc, int *id, int *posX, int *posY) {
   
    // Calcola la posizione relativa della rana rispetto al coccodrillo
    *posX = obj->coordinate_obj[1] - crocodiles[*crocodile_index].coordinate_obj[1];
    *posY = crocodiles[*crocodile_index].coordinate_obj[0];

    // Aggiorna l'ID del coccodrillo su cui si trova la rana
    *id = crocodiles[*crocodile_index].id;

    // Disattiva il flag 
    *isFirstTimeOnCroc = FALSE;
    
}

void frog_already_on_croc(Obj *obj , Obj crocodiles[NUM_COCCODRILLI], int crocodile_index, int *posX, int *posY){

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
        case KEY_UP:
            *posY -= VERTICAL_MOV;
            break;
        case KEY_DOWN:
            *posY += VERTICAL_MOV;
            break;
    }

}



