# Compiler
CC = gcc

# Project Name
TARGET = BeatCube

# Source Files
SRC = main.c

# Flags
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11

# Default Target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean Build Artifacts
clean:
	rm -f $(TARGET)

# Run Application
run: $(TARGET)
	./$(TARGET)

# Phony Targets
.PHONY: all clean run
