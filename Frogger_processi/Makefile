# Makefile

# Process_project
Process_project: Project.o father.o frog.o granade.o crocodile.o bullet.o menu.o procedure_generali.o
	gcc Project.o  father.o frog.o granade.o crocodile.o bullet.o menu.o procedure_generali.o -o Process_project -lncurses

# Regola per Project.c
Project.o: Project.c macro_struct.h father.h frog.h granade.h crocodile.h bullet.h menu.h procedure_generali.h
	gcc -c Project.c -o Project.o -lncurses

# Regola per father.c
father.o: father.c father.h macro_struct.h frog.h granade.h crocodile.h bullet.h procedure_generali.h
	gcc -c father.c -o father.o -lncurses

# Regola per frog.c
frog.o: frog.c frog.h macro_struct.h  granade.h procedure_generali.h
	gcc -c frog.c -o frog.o -lncurses

# Regola per granade.c
granade.o: granade.c granade.h macro_struct.h procedure_generali.h crocodile.h
	gcc -c granade.c -o granade.o -lncurses

# Regola per crocodile.c
crocodile.o: crocodile.c crocodile.h macro_struct.h bullet.h procedure_generali.h
	gcc -c crocodile.c -o crocodile.o -lncurses

# Regola per bullet.c
bullet.o: bullet.c bullet.h macro_struct.h 
	gcc -c bullet.c -o bullet.o -lncurses

# Regola per menu.c
menu.o: menu.c menu.h macro_struct.h procedure_generali.h 
	gcc -c menu.c -o menu.o -lncurses

# Regola per procedure_generali.c
procedure_generali.o: procedure_generali.c procedure_generali.h macro_struct.h 
	gcc -c procedure_generali.c -o procedure_generali.o -lncurses

# Regola per pulire i file oggetto e l'eseguibile
clean:
	rm -f Process_project *.o
