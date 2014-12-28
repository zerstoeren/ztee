CC = gcc
CFLAGS = -std=c99 -fpic -Wall -Wextra

all: queue.o ztee

ztee: ztee_main.c
	$(CC) $(CFLAGS) queue.o ztee_main.c -o ztee

queue.o: queue.h queue.c
	$(CC) $(CFLAGS) -c queue.c

clean:
	rm -f queue.o ztee
