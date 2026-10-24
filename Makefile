
# Project configuration
CMAKE := cmake
BUILD_DIR := build
EXECUTABLE := tuix

# Dependencies
SDL3_PREFIX := $(shell brew --prefix sdl3)
SKIA_DIR ?= $(HOME)/dev/skia

# Commands
.PHONY: configure build run clean

# Configure CMake
configure:
	$(CMAKE) -S . -B $(BUILD_DIR) \
		-DCMAKE_PREFIX_PATH="$(SDL3_PREFIX)" \
		-DSKIA_DIR="$(SKIA_DIR)"

# Build project
build: configure
	$(CMAKE) --build $(BUILD_DIR)

# Build and run
run: build
	./$(BUILD_DIR)/$(EXECUTABLE)

# Remove build files
clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)
