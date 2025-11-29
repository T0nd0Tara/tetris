tetris: main.c frame.o frame.h
	gcc main.c -o tetris -lncurses -L. -l:frame.o

frame.o: frame.c
	gcc -c $< -o $@
