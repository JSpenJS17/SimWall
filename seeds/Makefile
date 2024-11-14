# Author: Holden Vail
# Seeds Makefile

CC = gcc
CFLAGS = -Wall
TARGET = seeds
SRC = seeds.c
HEADERS = seeds.h

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET)
