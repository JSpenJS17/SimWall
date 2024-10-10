LIBS = -lX11 -lXext -lXrender
OUTNAME = simwall
CFILES = $(shell find . -name "*.c")

all:
	gcc $(CFILES) $(LIBS) -o $(OUTNAME)

clean:
	rm -f $(OUTNAME)
