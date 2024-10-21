LIBS = -lgdi32 -lopengl32
OUTNAME = simwall.exe
CFILES = simwall.c windows_lib.c brians_brain/brians_brain.c game_of_life/game_of_life.c

all:
	gcc -o $(OUTNAME) $(CFILES) $(LIBS)

clean:
	del /f $(OUTNAME)
