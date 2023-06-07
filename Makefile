structure:
	mkdir -p bin
	mkdir -p obj
	
bin/main: obj/main.o obj/die.o
	gcc -o bin/main obj/main.o obj/die.o

obj/main.o: src/main.c
	gcc -o obj/main.o -D_DEFAULT_SOURCE -c src/main.c

obj/die.o: lib/die/die.c
	gcc -o obj/die.o -c lib/die/die.c
	

install: bin/main
	echo "alias cmon=\"${PWD}/bin/main\"" >> ~/.bash_aliases
