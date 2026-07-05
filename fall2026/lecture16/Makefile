RUSTC ?= rustc
BIN_DIR = bin
SOURCES = $(wildcard *.rs)
TARGETS = $(patsubst %.rs,$(BIN_DIR)/%,$(SOURCES))

.PHONY: all clean help

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%: %.rs | $(BIN_DIR)
	$(RUSTC) $< -o $@

clean:
	rm -rf $(BIN_DIR)

help:
	@echo "Lecture 16 Rust examples"
	@echo "------------------------"
	@echo "  all    - Compile all .rs files into bin/"
	@echo "  clean  - Remove compiled binaries"
	@echo "  help   - Show this message"
