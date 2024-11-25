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

# Run the make command in the detected OS directory
build:
	@echo "---Building for $(detected_OS)---\n"
	@$(MAKE) -C $(detected_OS)
