all: fairp2p.o perfectp2p.o p1 p2

p1: fairp2p.o perfectp2p.o p1.c
	gcc -o p1 fairp2p.o perfectp2p.o p1.c -std=c99 -lpthread

p2: fairp2p.o perfectp2p.o p2.c
	gcc -o p2 fairp2p.o perfectp2p.o p2.c -std=c99 -lpthread

perfectp2p.o: perfectp2p.c perfectp2p.h
	gcc -c -o perfectp2p.o perfectp2p.c
	
fairp2p.o: fairp2p.c fairp2p.h
	gcc -c -o fairp2p.o fairp2p.c

clean:
	rm -rf *.o p1 p2
