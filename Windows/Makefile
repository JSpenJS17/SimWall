LIBS = -lgdi32 -lopengl32
CFILES = simwall.c windows_lib.c seeds/seeds.c langtons_ant/langtons_ant.c brians_brain/brians_brain.c game_of_life/game_of_life.c

all:
	gcc -o $(OUTNAME) $(CFILES) $(LIBS)

clean:
	del /f $(OUTNAME)
