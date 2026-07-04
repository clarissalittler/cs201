RUSTC ?= rustc
BIN_DIR = bin
# echo2.rs is *deliberately* broken (non-exhaustive match) to show the
# compiler catching it -- filter it out so `make` builds everything else
SOURCES = $(filter-out echo2.rs,$(wildcard *.rs))
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
	@echo "Rust Intro Makefile"
	@echo "-------------------"
	@echo "Available targets:"
	@echo "  all    - Compile all standalone Rust examples into bin/"
	@echo "  clean  - Remove compiled binaries"
	@echo "  help   - Show this message"
