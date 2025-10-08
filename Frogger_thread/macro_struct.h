#ifndef MACRO_STRUCT_H
#define MACRO_STRUCT_H

#include <unistd.h>   // Include per funzioni come fork, pipe, e sleep
#include <stdlib.h>   // Include per funzioni di gestione memoria e conversione
#include <ncurses.h>  // Include per la gestione del terminale (grafica testuale)
#include <time.h>     // Include per funzioni di gestione del tempo (es. srand, rand)
#include <stdbool.h>  // Include per il tipo booleano `bool`
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


extern int tempo;

#define BUFFER_SIZE 128


//FINESTRA GIOCO
#define WIDTH_WINDOW 61
#define HEIGHT_WINDOW 29
#define CENTER_Y ((LINES - HEIGHT_WINDOW )/2)
#define CENTER_X ((COLS - WIDTH_WINDOW )/2)



//TANE
#define HOLE_POSITION_X 3
#define WIDTH_HOLE 7
#define INIZIO_HOLE 2
#define NUM_HOLE 5
#define HOLE_HEIGHT HOLE_POSITION_X 



//FIUME
#define END_FIUME 8
#define START_FIUME 22



//FINESTRA INFO
#define HEIGHT_INFO 3
#define DATI_INFO_Y 1
#define TIMER_INFO_X  (WIDTH_WINDOW-12)
#define LIVES_INFO_X 2
#define SCORE_INFO_X ((WIDTH_WINDOW /2)-13)
#define ORIGIN_Y (CENTER_Y + HEIGHT_WINDOW)
#define TEMPO 100



//SEZIONE FROG
#define FROG_Y  (HEIGHT_WINDOW-3) 
#define FROG_X  ((WIDTH_WINDOW/2)-1)
#define HEIGHT_FROG 2
#define WIDTH_FROG 3
#define VERTICAL_MOV 2
#define HORIZONTAL_MOV 2 
#define UPPER_LIMIT 2
#define LOWER_LIMIT 3
#define LEFT_LIMIT 1
#define RIGHT_LIMIT 4
#define QUIT -1
#define MOVE 1
#define MAX_GRANATE 10
#define SINISTRA  (HEIGHT_WINDOW-3) 
#define DESTRA  (WIDTH_WINDOW/2-1)



//SEZIONE CROCODILE
#define CROC_Y  (HEIGHT_WINDOW-7)
#define HEIGHT_CROC 2
#define WIDTH_CROC 8
#define NUM_COCCODRILLI 16
#define VELOCITA_FLUSSI 8
#define NUM_FLUSSI 8
#define FIRST_OF_THE_COUPLE 0
#define SECOND_OF_THE_COUPLE 1



//COLORI
#define WHITE_BLACK 1
#define BLUE_BLUE 2
#define RED_GREEN 3
#define RED_BLACK 4
#define YELLOW_BLACK 5
#define GREEN_BLACK 6
#define RED_BLUE 7
#define BLACK_BLUE 8
#define GREEN_WHITE 9
#define MAGENTA_BLACK 10
#define BLACK_MAGENTA 11
#define RED_MAGENTA 12
#define WHITE_MAGENTA 13
#define YELLOW_BLUE 14
#define COLOR_ORANGE 15
#define BLACK_ORANGE 16
#define ORANGE_BLACK 17
#define COLOR_GOLD 18
#define GOLD_RED 19



//PERSONAGGI
#define RANA 0
#define GRANATA 1
#define COCCODRILLO 2
#define PROIETTILE 3



//MENU FINE_PARTITA
#define HEIGHT_BOX 7
#define WIDTH_BOX 53
#define POS_X_BOX 4
#define POS_Y_BOX 3
#define VERTICAL_MOV_BOX 9
#define HORIZONTAL_MOV_BOX 20
#define COORDINATA_Y_BOX_1 6
#define COORDINATA_Y_BOX_2 15
#define COORDINATA_X_STRINGA_BOX1 18
#define COORDINATA_X_STRINGA_BOX2 27



//SMALL BOX
#define NUM_SMALL_BOX 3
#define COORDINATA_Y_SMALL_BOX 22
#define COORDINATA_X_EASY 10
#define COORDINATA_X_MEDIUM 30
#define COORDINATA_X_HARD 50
#define HEIGHT_SMALL_BOX 5
#define MAX_Y_SMALL_BOX 25
#define START_EASY 5
#define END_EASY 20
#define COORDINATA_X_STRINGA_EASY 9
#define START_MEDIUM 23
#define END_MEDIUM 38
#define COORDINATA_X_STRINGA_MEDIUM  MAX_Y_SMALL_BOX
#define START_HARD 41
#define END_HARD 56
#define COORDINATA_X_STRINGA_HARD 45
#define SPAWN_X 30
#define SPAWN_Y 6
#define POS_Y_RISULTATO_PARTITA 19
#define POS_X_RISULTATO_PARTITA 5
#define POS_X_SCORE_PARTITA 43
#define INFOSTARTY 26
#define INFOSTARTX 12



//MODALITA
//EASY
#define SPEED_CROC_MIN_ES 100000
#define SPEED_CROC_MAX_ES 170000
#define SPEED_BULLET_ES 50000
#define MIN 3
#define MAX_ES 5



//MEDIUM
#define SPEED_CROC_MIN_MD 80000
#define SPEED_CROC_MAX_MD 110000
#define SPEED_BULLET_MD 40000
#define MAX_MD 4



//HARD 
#define SPEED_CROC_MIN_HD 40000
#define SPEED_CROC_MAX_HD 80000
#define SPEED_BULLET_HD 30000
#define MAX_HD 3



//STRUTTURE DATI
typedef struct{
    
    int vite;
    int coordinate_obj[2];
    int coordinate_granata[3];
    char granade;
    char sprite_frog [HEIGHT_FROG][WIDTH_FROG];
    char sprite_cocc [HEIGHT_CROC][WIDTH_CROC];
    int ch;
    int score;
    int id;
    bool direction_cocc;
    int speed_cocc;
    int tipo;
    bool new_manche;
    pthread_t thread_id;
    int tane_visitate;
    bool tana_visitata;
    int centro_tana;
    int end_game[2];

}Obj;


    
typedef struct {
    bool reset_flag;
    int velocita[VELOCITA_FLUSSI];
} ResetStruct;



typedef struct {

    int vite_rana;
    int speed_crocodile_min;
    int speed_crocodile_max;
    int speed_bullet;
    int t_period_spawn_bullet;
    
}Mode;



typedef struct {

    bool choice;
    bool easy;
    bool medium;
    bool hard;

}Menu;    



// Buffer produttore-consumatore
typedef struct {
    Obj buffer[BUFFER_SIZE]; // Array di oggetti che rappresenta il buffer circolare
    int in;                  // indice di inserimento
    int out;                 // indice di estrazione
    pthread_mutex_t mutex;   // mutex per proteggere gli accessi al buffer
    sem_t empty;             // semaforo per posti vuoti
    sem_t full;              // semaforo per posti pieni
} SharedBuffer;



typedef struct {
    Obj *frog;
    Mode game_mode;
} F_Args;



typedef struct {
    Obj *crocodile;
    Mode game_mode;
} C_Args;



typedef struct {
    Obj *bullet;
    Mode game_mode;
} B_Args;



// Variabili globali condivise
extern SharedBuffer shared_buffer;
extern pthread_mutex_t game_state_mutex;
extern bool game_running;
extern bool game_paused;
extern int  crocodile_offset_x;
extern pthread_mutex_t coordinate_mutex;
extern WINDOW *fixed_win, *info;
// Matrice per cancellazione sfondo
extern int sfondo_colori[HEIGHT_WINDOW][WIDTH_WINDOW];
extern char sfondo_carattere[HEIGHT_WINDOW][WIDTH_WINDOW];

extern sem_t frog_on_croc_sem;
extern sem_t reset_sem;
extern sem_t kill_bullet_sem;
extern sem_t kill_granade_sem;
extern pthread_mutex_t ncurses_mutex ;
extern pthread_mutex_t flow_speed_mutex;
extern pthread_mutex_t offset_mutex;
extern int flow_speed[NUM_COCCODRILLI/2];




#endif 

