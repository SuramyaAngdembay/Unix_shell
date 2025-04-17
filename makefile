# Makefile for osh-shell

# Compiler
CC = gcc

# Directories
SRC_DIR = src
BIN_DIR = bin

# Files
SRC = $(SRC_DIR)/osh.c
OUT = $(BIN_DIR)/osh

# Default target
all: $(OUT)

# Compile the program
$(OUT): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(SRC) -o $(OUT)

# Run the program
run: all
	./$(OUT)

# Remove compiled files
clean:
	rm -rf $(BIN_DIR)

