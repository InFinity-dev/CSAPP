CC = gcc
CFLAGS = -O2 -Wall -I .

LIB = -lpthread

all: echoserveri echoclient

echoserveri: echoserveri.c csapp.o
	$(CC) $(CFLAGS) -o echoserveri echoserveri.c csapp.o $(LIB)

echoclient: echoclient.c csapp.o
	$(CC) $(CFLAGS) -o echoclient echoclient.c csapp.o $(LIB)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c


clean:
	rm -f *.o echo *~
