BINNAME = simwall_cmd

# Detect OS and set variables
ifeq ($(OS),Windows_NT)
    detected_OS := Windows
    OUTNAME := $(abspath $(BINNAME).exe)
    OUTNAME := $(subst \,/,$(OUTNAME))
    COPY_CMD := powershell -Command "Copy-Item -Path '$(OUTNAME)' -Destination 'electron/sim_wall/execs/$(detected_OS)/$(BINNAME).exe'"
    MKDIR_CMD := powershell -Command "if (!(Test-Path 'electron/sim_wall/execs/$(detected_OS)')) { New-Item -ItemType Directory -Path 'electron/sim_wall/execs/$(detected_OS)' }"
else
    detected_OS := $(shell uname)
    ifeq ($(detected_OS), Darwin)
        detected_OS := MacOS
    endif
    OUTNAME := $(abspath $(BINNAME))
    COPY_CMD := cp "$(OUTNAME)" "electron/sim_wall/execs/$(detected_OS)/$(BINNAME)"
    MKDIR_CMD := mkdir -p "electron/sim_wall/execs/$(detected_OS)"
endif

# Trim OUTNAME to remove spaces
OUTNAME := $(strip $(OUTNAME))

all: build

# Build target
build:
	@echo ---Building for $(detected_OS)---

# Ensure the output directory exists
	@$(MKDIR_CMD)

# Copy the binary to the destination directory
	@$(COPY_CMD)
