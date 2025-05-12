CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Source directories
FORK_DIR = fork-examples
THREAD_DIR = thread-examples
SYNC_DIR = synchronization-examples
IPC_DIR = ipc-examples
COMMENTED_DIR = commented-examples

# Output directories
BIN_DIR = bin
BIN_FORK_DIR = $(BIN_DIR)/$(FORK_DIR)
BIN_THREAD_DIR = $(BIN_DIR)/$(THREAD_DIR)
BIN_SYNC_DIR = $(BIN_DIR)/$(SYNC_DIR)
BIN_IPC_DIR = $(BIN_DIR)/$(IPC_DIR)
BIN_COMMENTED_DIR = $(BIN_DIR)/$(COMMENTED_DIR)

# Find all source files
FORK_SRCS = $(wildcard $(FORK_DIR)/*.c)
THREAD_SRCS = $(wildcard $(THREAD_DIR)/*.c)
SYNC_SRCS = $(wildcard $(SYNC_DIR)/*.c)
IPC_SRCS = $(wildcard $(IPC_DIR)/*.c)
COMMENTED_SRCS = $(wildcard $(COMMENTED_DIR)/0*/*.c)

# Generate executable names
FORK_EXECS = $(patsubst $(FORK_DIR)/%.c,$(BIN_FORK_DIR)/%,$(FORK_SRCS))
THREAD_EXECS = $(patsubst $(THREAD_DIR)/%.c,$(BIN_THREAD_DIR)/%,$(THREAD_SRCS))
SYNC_EXECS = $(patsubst $(SYNC_DIR)/%.c,$(BIN_SYNC_DIR)/%,$(SYNC_SRCS))
IPC_EXECS = $(patsubst $(IPC_DIR)/%.c,$(BIN_IPC_DIR)/%,$(IPC_SRCS))
COMMENTED_EXECS = $(patsubst $(COMMENTED_DIR)/%.c,$(BIN_COMMENTED_DIR)/%,$(COMMENTED_SRCS))

# Default target
all: directories original commented

# Build original examples
original: $(FORK_EXECS) $(THREAD_EXECS) $(SYNC_EXECS) $(IPC_EXECS)

# Build commented examples
commented: $(COMMENTED_EXECS)

# Create bin directories
directories:
	@mkdir -p $(BIN_FORK_DIR)
	@mkdir -p $(BIN_THREAD_DIR)
	@mkdir -p $(BIN_SYNC_DIR)
	@mkdir -p $(BIN_IPC_DIR)
	@mkdir -p $(BIN_COMMENTED_DIR)/01-basic-processes
	@mkdir -p $(BIN_COMMENTED_DIR)/02-advanced-processes
	@mkdir -p $(BIN_COMMENTED_DIR)/03-basic-threads
	@mkdir -p $(BIN_COMMENTED_DIR)/04-thread-synchronization
	@mkdir -p $(BIN_COMMENTED_DIR)/05-ipc

# Compilation rules for original examples
$(BIN_FORK_DIR)/%: $(FORK_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_THREAD_DIR)/%: $(THREAD_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_SYNC_DIR)/%: $(SYNC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_IPC_DIR)/%: $(IPC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Compilation rules for commented examples
$(BIN_COMMENTED_DIR)/%: $(COMMENTED_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Clean target
clean:
	@rm -rf $(BIN_DIR)
	@echo "Cleaned compiled executables"

# Help target
help:
	@echo "Concurrency Tutorial Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build all examples (default)"
	@echo "  original   - Build only the original examples"
	@echo "  commented  - Build only the commented examples"
	@echo "  clean      - Remove all built executables"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make                   # Build everything"
	@echo "  make original          # Build only original examples"
	@echo "  make commented         # Build only commented examples"
	@echo "  ./bin/fork-examples/fork1  # Run a specific example"

.PHONY: all original commented directories clean help