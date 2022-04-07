build: avion

avion: tema2.c
	gcc -Wall -Wextra -g tema2.c -o avion

run:
	./avion

clean:
	rm -f avion