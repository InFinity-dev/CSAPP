CC = gcc
CFLAGS = -O2 -Wall -I .

LIB = -lpthread

all: hostinfo csapp

hostinfo: hostinfo.c csapp.o
	$(CC) $(CFLAGS) -o hostinfo hostinfo.c csapp.o $(LIB)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm -f *.o echo *~
