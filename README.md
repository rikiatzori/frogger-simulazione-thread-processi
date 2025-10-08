# Frog Crossing Simulation - Progetto Sistemi Operativi

## Autori
Giovanni Scano 
Riccardo Atzori 

## Descrizione
Simulazione didattica ispirata al classico gioco arcade Frogger, sviluppata per il corso di Sistemi Operativi.  
Il progetto implementa due versioni distinte per lo studio di thread e processi.

**Nota sul Copyright**: Questo progetto è una simulazione didattica sviluppata per scopi educativi.  
Il gioco è ispirato al classico Frogger © Konami, ma implementato con fini puramente educativi e di studio dei sistemi operativi.

### File Sonori
I suoni utilizzati in questo progetto sono stati presi da [classicgaming.cc/classics/frogger/sounds](https://classicgaming.cc/classics/frogger/sounds) e rappresentano i suoni originali del gioco Frogger © Konami.

**Nota legale**: Questi file audio sono utilizzati esclusivamente per scopi educativi e didattici in questo progetto universitario non commerciale.



Makefile:

In entrambe le cartelle sono presenti i Makefile con l'eseguibile già compilato. Se si volesse comunque procedere con il comando make, va fatto nella directory in cui è presente il Makefile.


Avviso Importante:

Quando si esegue il file, ovvero l'eseguibile di gioco, assicurarsi che il terminale sia a tutto schermo; in caso di finestra minimizzata, il gioco presenterà grossi problemi grafici e non funzionerà in modo corretto.


Funzionalità Bonus:

Menu di Gioco
Menu inizio/fine partita.
Controlli di Gioco
Pausa: Premere "q" o "Q"
Ripresa: Premere "p" o "P"

Modalità di Difficoltà
Il gioco offre tre livelli di difficoltà crescente:
-Easy
-Medium
-Hard
Ogni modalità presenta diverse impostazioni per:

Numero di vite
-Intervallo di spawn dei proiettili
-Velocità dei proiettili
-Velocità del flusso dei coccodrilli
-Suoni e Musiche

Per la funzionalità audio, raccomandiamo di installare i seguenti pacchetti di sistema tramite questi comandi:
-sudo apt update
-sudo apt install alsa-utils
-sudo apt install mpg123

L'audio del gioco include:
-Musica di sottofondo (si interrompe quando il gioco è in pausa)
-Effetti sonori per:
  °Entrata della rana in tana
  °Caduta della rana nel fiume
  °Rana colpita da un proiettile


Istruzioni di Gioco:

Di default, nel menu di inizializzazione, premendo "NUOVA PARTITA" la partita inizierà automaticamente in modalità EASY.
Si naviga nei menu con le frecce direzionali e si conferma con ENTER.
La rana può sparare solo dall'inizio del marciapiede fino alla fine del fiume. Una volta che la rana entra nella "safe zone", non potrà più sparare.

Sistema di Punteggio:

-Ogni entrata in una tana mai visitata attribuirà 20 punti più il tempo rimanente della manche.
-Vengono detratti 10 punti ogni volta che:
  °La rana cade in acqua
  °La rana viene colpita da un proiettile
  °Scade il tempo
  °La rana entra in una tana già visitata
  °La rana entra tra le tane

Ad ogni fine partita, in caso di vittoria, il punteggio finale sarà 50 moltiplicato per il numero di vite rimanenti.


## ⚠️ Licenza e Avviso sul Copyright

**UTILIZZO ESCLUSIVAMENTE EDUCATIVO**

Questo progetto è rilasciato sotto Licenza per Soli Scopi Educativi.
Sebbene abbiamo implementato una versione tecnica per fini accademici:

- Nome Frogger, concept e meccaniche di gioco sono © **Konami**
- File audio sono © **Konami** e utilizzati in base al fair use educativo
- **È vietato l'uso commerciale, la distribuzione o la modifica**
- Questo progetto è dedicato allo **studio dei sistemi operativi** in ambito accademico
