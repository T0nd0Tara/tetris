BUILD=bin

$(BUILD)/tetris: $(BUILD) main.c $(BUILD)/frame.o frame.h
	gcc main.c -o $@ -lncurses -L./$(BUILD)/ -l:frame.o

$(BUILD)/frame.o: $(BUILD) frame.c frame.h
	gcc -c frame.c -o $@

$(BUILD):
	mkdir -p $(BUILD)
