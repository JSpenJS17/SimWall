TARGET = game_of_life
SRC = game_of_life.c
FILE = start_pattern.txt
GENERATIONS = 300


all: $(TARGET)
	gcc -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) $(FILE) $(GENERATIONS)

clean:
	rm -f $(TARGET)
