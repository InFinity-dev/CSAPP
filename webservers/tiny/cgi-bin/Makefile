CC = gcc
CFLAGS = -O2 -Wall -I ..

all: adder post-adder

adder: adder.c
	$(CC) $(CFLAGS) -o adder adder.c

post-adder: post-adder.c
	$(CC) $(CFLAGS) -o post-adder post-adder.c

clean:
	rm -f adder post-adder *~
