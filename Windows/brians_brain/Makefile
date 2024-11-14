CC = gcc
CFLAGS = -Wall -g
TARGET = brians_brain

all: $(TARGET)

$(TARGET): brians_brain.c
	$(CC) $(CFLAGS) -o $(TARGET) brians_brain.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)