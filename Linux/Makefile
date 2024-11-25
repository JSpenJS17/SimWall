LIBS = -lX11
OUTNAME = ../simwall
CFILES = $(shell find . -name "*.c")
CFLAGS = -Wall -O2

all:
	gcc $(CFILES) $(LIBS) $(CFLAGS) -o $(OUTNAME)

clean:
	rm -f $(OUTNAME)
