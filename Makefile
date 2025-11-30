BUILD=bin

$(BUILD)/tetris: main.c $(BUILD)/frame.o frame.h | $(BUILD)
	gcc main.c -o $@ -lncurses -L./$(BUILD)/ -l:frame.o -lm

$(BUILD)/frame.o: frame.c frame.h | $(BUILD) 
	gcc -c frame.c -o $@

$(BUILD):
	mkdir -p $(BUILD)
