a.out: main.o final.o
	gcc -o a.out main.o final.o

main.o: main.c final.h
	gcc -c main.c

final.o: final.c final.h
	gcc -c final.c

clean:
	rm a.out *.o