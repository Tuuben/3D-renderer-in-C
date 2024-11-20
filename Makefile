build:
	gcc -Wall -std=c99 $(shell pkg-config --cflags sdl2) -lm ./src/*.c -o renderer $(shell pkg-config --libs sdl2) 

run:
	./renderer

clean:
	rm renderer