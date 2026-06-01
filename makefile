# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g3 -fsanitize=address -fsanitize=undefined
LIBS = -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf

# Build directory
BUILD_DIR = build

# Source files
SOURCES = main.c \
          engine/src/game.c \
          engine/src/setup.c \
          engine/src/input.c \
          engine/src/renderer.c \
          engine/src/update.c \
          engine/src/commons.c

# Object files (in build directory)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.o))

# Output
TARGET = game

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/engine/src

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Rebuild
rebuild: clean all

.PHONY: all clean rebuild