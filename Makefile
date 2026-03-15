# Compiler and Flags
CC = gcc 
CFLAGS = -Wall -Wextra -std=c99 -O2 -I./include 
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11 

# Project Name
TARGET = BeatCube 

# Find all .c files in the src directory
SRC = $(wildcard src/*.c)
# Convert .c filenames to .o filenames in the obj directory
OBJ = $(SRC:src/%.c=obj/%.o)

# Default Target
all: prepare $(TARGET) 

# Create necessary directories
prepare:
	@mkdir -p obj bin

# Link the final binary
$(TARGET): $(OBJ) 
	$(CC) $(CFLAGS) -o bin/$(TARGET) $(OBJ) $(LDFLAGS) 

# Compile source files into object files
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Build Artifacts
clean: 
	rm -rf obj bin 

# Run Application
run: all 
	./bin/$(TARGET) 

.PHONY: all clean run prepare
