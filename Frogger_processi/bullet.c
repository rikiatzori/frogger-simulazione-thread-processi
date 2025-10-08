#include "bullet.h"
#include "macro_struct.h"



/* Funzione che si occupa di far muovere i proiettili dei coccodrilli, per ogni proiettile acquisiamo il pid che ci servirà per la sua uccisione e per metterli in pausa.
Quando escono dai bordi gli vengono assegnate delle coordinate di default */
void bullet_process(int p_childs_to_father[2], int coordinata_y, int coordinata_x, int id, bool direzione, int speed_bullet) {

    close(p_childs_to_father[0]);

    Obj bullet = {
        .id = id,
        .coordinate_obj = {coordinata_y, coordinata_x},
        .granade = 'o',
        .direction_cocc = direzione,
        .tipo = PROIETTILE,
    };

    //questo mi serve per i coccodrilli che son rivolti verso  destra, "sposto" la x di 8 caratteri
    if (bullet.direction_cocc == FALSE){

        bullet.coordinate_obj[1] += 8;
    }else{

        //coccodrillo rivolto verso sinistra
        bullet.coordinate_obj[1] -= 1;
    }

    //prendo il pid del proiettile
    bullet.process_pid = getpid();

    while (bullet.coordinate_obj[1] > 1 && bullet.coordinate_obj[1] <= WIDTH_WINDOW - 4){

        // Aggiorna la posizione del bullet
        if (bullet.direction_cocc == TRUE){
            bullet.coordinate_obj[1] -= 1;
        }else{
            bullet.coordinate_obj[1] += 1;
        }

        write(p_childs_to_father[1], &bullet, sizeof(Obj));
        usleep(speed_bullet);
    }

    bullet.coordinate_obj[0] = -1; // Coordinate fuori dallo schermo
    bullet.coordinate_obj[1] = -1; 
    bullet.granade = ' ';  // Nessun proiettile visibile

    write(p_childs_to_father[1], &bullet, sizeof(Obj));

    close(p_childs_to_father[1]);
    exit(0);
}


// Funzione che serve epr l'uccisione dei proiettili dei coccodrillir
void kill_bullet(int p_childs_to_father[2], Obj crocodile, int *pid_bullet, bool *flag_bullet, time_t *next_bullet_time){

    // se arriva il reset del tempo, e il processo proiettile `e ancora in esecuzione
    if (*flag_bullet == TRUE){

        // creo una struttura bullet  che rappresentera il proiettile da eliminare
        Obj bullet_killed = {

            .id = crocodile.id,           // prendiamo l id del coccodrillo per sapere di che proiettile si tratta nel vettore bullet[]
            .coordinate_obj = {ERR, ERR}, // usiamo delle coordinate di reset
            .tipo = PROIETTILE,
            .granade = ' ' // usiamo lo spazio per riconoscere il proiettile ucciso nel padre e inoltre lo cancelliamo
        };

        // Il coccodrillo invia il messaggio di rimozione del proiettile al padre
        write(p_childs_to_father[1], &bullet_killed, sizeof(Obj));

        // effettuo la terminazione del figlio
        kill(*pid_bullet, SIGKILL);
        waitpid(*pid_bullet, NULL, 0);

        *flag_bullet = FALSE;                               // indico che il processo bullet non e piu in esecuzione
        *pid_bullet = 0;                                    // Azzera il pid per sicurezza 
        *next_bullet_time = time(NULL) + (rand() % 10) + 2; // Reset intervallo di sparo
    }
}
