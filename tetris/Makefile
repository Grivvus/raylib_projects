SHELL = /bin/bash
main: run

run: build 
	./bin/game

build: 
	gcc -o ./bin/game ./src/tetris.c -lraylib
