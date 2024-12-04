LIBS = -lX11
CFILES = $(shell find . -name "*.c")
CFLAGS = -Wall -O2

all:
	gcc $(CFILES) $(LIBS) $(CFLAGS) -o $(OUTNAME)

clean:
	rm -f $(OUTNAME)
