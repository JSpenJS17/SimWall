# Detect OS without assuming uname is available
BINNAME = simwall_cmd


ifeq ($(OS),Windows_NT)
    detected_OS := Windows
    OUTNAME := $(abspath $(BINNAME))  # Get the absolute path
else
    detected_OS := $(shell uname)
    ifeq ($(detected_OS), Darwin)
        detected_OS := MacOS
    endif
    OUTNAME := $(shell pwd)/$(BINNAME)
endif

# Trim OUTNAME to remove all spaces at the end
OUTNAME := $(strip $(OUTNAME))

all: build


# Run the make command in the detected OS directory
build:
	@echo ---Building for $(detected_OS)---
	@echo OUTNAME: "$(OUTNAME)"

	@$(MAKE) -C $(detected_OS) OUTNAME="$(OUTNAME)"

	@mkdir -p "electron/sim_wall/execs/$(detected_OS)"
	@cp "$(OUTNAME)" "electron/sim_wall/execs/$(detected_OS)/$(BINNAME)"
