# FILE: MacOS/Makefile

# Name of the output binary
OUTNAME ?= simwall_cmd

# Path to the precompiled executable
PRECOMPILED_EXEC := ./SimWall/build/SimWall

# "Build" target
all:
	cp $(PRECOMPILED_EXEC) $(OUTNAME)

clean:
	rm -rf $(OUTNAME)