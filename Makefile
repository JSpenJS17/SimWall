LIBS = -lX11 -lXext -lXrender
OUTNAME = simwall
CFILE = simwall_demo.c

all:
	gcc $(CFILE) $(LIBS) -o $(OUTNAME)

clean: 
	rm -f $(OUTNAME)