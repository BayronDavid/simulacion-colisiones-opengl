CC = gcc
CFLAGS = -Wall -O2 -I/usr/include -I/usr/include/SDL2
LDFLAGS = -lSDL2 -lGLEW -lGL -lGLU -lm

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main $(LDFLAGS)

clean:
	rm -f main
