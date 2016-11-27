playerpiano : main.o
	gcc `pkg-config --cflags --libs opencv gtk+-2.0 gmodule-export-2.0 smf` main.o -o playerpiano

main.o : main.c
	gcc `pkg-config --cflags --libs opencv gtk+-2.0 gmodule-export-2.0 smf` -c main.c -o main.o

clean :
	rm *.o playerpiano
