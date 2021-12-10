verse_grab: main.c json.o
	gcc -o verse_grab main.c json.o -l curl -lm
json.o: json.c json.h
	gcc -o json.o -c json.c -lm
clean:
	rm verse_grab json.o
