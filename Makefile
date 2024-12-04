# Detect OS without assuming uname is available
ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname)
    ifeq ($(detected_OS), Darwin)
        detected_OS := MacOS
    endif
endif

all: build

OUTNAME = $(shell pwd)/simwall_cmd

# Run the make command in the detected OS directory
build:
	@echo ---Building for $(detected_OS)---
    
    # Run the make command in the detected OS directory
	@$(MAKE) -C $(detected_OS) OUTNAME=$(OUTNAME)

    # Copy the executable to the electron directory
	@mkdir -p electron/sim_wall/execs/$(detected_OS)
	@cp simwall electron/sim_wall/execs/$(detected_OS)/simwall