# CS201 Systems Programming - Top-Level Makefile
# This Makefile provides convenient targets for building all examples across the repository

.PHONY: all help clean test tutorials exercises lectures fall2024 fall2025 winter2025 spring2025

# Default target
all: help

# Help target - show available commands
help:
	@echo "CS201 Systems Programming Repository - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  help              - Show this help message (default)"
	@echo "  all-examples      - Build ALL examples in the repository (may take time)"
	@echo "  tutorials         - Build all tutorial examples"
	@echo "  exercises         - Build all exercise sets"
	@echo "  lectures          - Build all lecture examples"
	@echo ""
	@echo "Semester-specific builds:"
	@echo "  fall2024          - Build all Fall 2024 lecture examples"
	@echo "  fall2025          - Build all Fall 2025 lecture examples"
	@echo "  winter2025        - Build all Winter 2025 lecture examples"
	@echo "  spring2025        - Build all Spring 2025 examples"
	@echo ""
	@echo "Tutorial-specific builds:"
	@echo "  concurrency       - Build concurrency tutorial examples"
	@echo "  assembly          - Build assembly examples"
	@echo "  ipc               - Build IPC lecture examples"
	@echo "  deadlock          - Build deadlock examples"
	@echo "  file-io           - Build file I/O examples"
	@echo "  sockets           - Build socket examples"
	@echo "  rust              - Build Rust examples"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean             - Clean all built executables (exercises clean only)"
	@echo "  clean-all         - Clean ALL built files in repository (use with caution)"
	@echo "  test              - Run build tests on major components"
	@echo ""
	@echo "Examples:"
	@echo "  make tutorials    # Build all tutorials"
	@echo "  make fall2025     # Build all Fall 2025 lectures"
	@echo "  make exercises    # Build all exercise sets"

# Build all examples in the entire repository
all-examples: tutorials exercises lectures
	@echo ""
	@echo "✓ All examples built successfully!"

# Tutorial builds
tutorials: concurrency assembly ipc deadlock file-io sockets
	@echo "✓ All tutorials built"

concurrency:
	@echo "Building concurrency tutorial..."
	@$(MAKE) -C concurrency-tutorial

assembly:
	@echo "Building assembly examples..."
	@$(MAKE) -C assembly-examples

ipc:
	@echo "Building IPC lecture examples..."
	@$(MAKE) -C ipc-lecture

deadlock:
	@echo "Building deadlock examples..."
	@$(MAKE) -C deadlock-lecture

file-io:
	@echo "Building file I/O examples..."
	@$(MAKE) -C file-intro

sockets:
	@echo "Building socket examples..."
	@$(MAKE) -C socket-intro

rust:
	@echo "Building Rust examples..."
	@cd rust-intro && cargo build

# Exercise builds
exercises:
	@echo "Building exercises..."
	@$(MAKE) -C concurrency-tutorial/exercises/01-basic-processes
	@$(MAKE) -C concurrency-tutorial/exercises/02-threads-intro
	@$(MAKE) -C concurrency-tutorial/exercises/03-synchronization
	@$(MAKE) -C concurrency-tutorial/exercises/04-ipc
	@$(MAKE) -C concurrency-tutorial/exercises/05-sockets
	@echo "✓ All exercises built"

# Lecture builds
lectures: fall2024 fall2025 winter2025 spring2025
	@echo "✓ All lectures built"

# Fall 2024 lectures
fall2024:
	@echo "Building Fall 2024 lectures..."
	@for dir in fall2024/lecture*; do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "  Building $$dir..."; \
			$(MAKE) -C "$$dir" || exit 1; \
		fi \
	done
	@echo "✓ Fall 2024 lectures built"

# Fall 2025 lectures
fall2025:
	@echo "Building Fall 2025 lectures..."
	@for dir in fall2025/lecture* fall2025/file-lecture fall2025/pointerExtras; do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "  Building $$dir..."; \
			$(MAKE) -C "$$dir" || exit 1; \
		fi \
	done
	@echo "✓ Fall 2025 lectures built"

# Winter 2025 lectures
winter2025:
	@echo "Building Winter 2025 lectures..."
	@for dir in winter2025/lecture* winter2025/finalproj; do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "  Building $$dir..."; \
			$(MAKE) -C "$$dir" || exit 1; \
		fi \
	done
	@echo "✓ Winter 2025 lectures built"

# Spring 2025 examples
spring2025:
	@echo "Building Spring 2025 examples..."
	@for dir in spring2025/assembly spring2025/forking spring2025/threading spring2025/in-class spring2025/video*; do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			echo "  Building $$dir..."; \
			$(MAKE) -C "$$dir" || exit 1; \
		fi \
	done
	@echo "✓ Spring 2025 examples built"

# Clean targets
clean:
	@echo "Cleaning exercises..."
	@$(MAKE) -C concurrency-tutorial/exercises/01-basic-processes clean 2>/dev/null || true
	@$(MAKE) -C concurrency-tutorial/exercises/02-threads-intro clean 2>/dev/null || true
	@$(MAKE) -C concurrency-tutorial/exercises/03-synchronization clean 2>/dev/null || true
	@$(MAKE) -C concurrency-tutorial/exercises/04-ipc clean 2>/dev/null || true
	@$(MAKE) -C concurrency-tutorial/exercises/05-sockets clean 2>/dev/null || true
	@echo "✓ Exercises cleaned"

clean-all: clean
	@echo "Cleaning all tutorials..."
	@$(MAKE) -C concurrency-tutorial clean 2>/dev/null || true
	@$(MAKE) -C assembly-examples clean 2>/dev/null || true
	@$(MAKE) -C ipc-lecture clean 2>/dev/null || true
	@$(MAKE) -C deadlock-lecture clean 2>/dev/null || true
	@$(MAKE) -C file-intro clean 2>/dev/null || true
	@$(MAKE) -C socket-intro clean 2>/dev/null || true
	@echo "Cleaning all lectures..."
	@for dir in fall2024/lecture* fall2025/lecture* fall2025/file-lecture fall2025/pointerExtras winter2025/lecture* winter2025/finalproj spring2025/*; do \
		if [ -d "$$dir" ] && [ -f "$$dir/Makefile" ]; then \
			$(MAKE) -C "$$dir" clean 2>/dev/null || true; \
		fi \
	done
	@echo "✓ All built files cleaned"

# Test build of major components
test:
	@echo "Testing builds of major components..."
	@echo ""
	@echo "1. Testing concurrency tutorial..."
	@$(MAKE) -C concurrency-tutorial > /dev/null 2>&1 && echo "   ✓ Concurrency tutorial builds successfully" || echo "   ✗ Concurrency tutorial build failed"
	@echo ""
	@echo "2. Testing exercises..."
	@$(MAKE) -C concurrency-tutorial/exercises/01-basic-processes > /dev/null 2>&1 && echo "   ✓ Basic processes exercises build successfully" || echo "   ✗ Basic processes exercises build failed"
	@$(MAKE) -C concurrency-tutorial/exercises/03-synchronization > /dev/null 2>&1 && echo "   ✓ Synchronization exercises build successfully" || echo "   ✗ Synchronization exercises build failed"
	@echo ""
	@echo "3. Testing sample lectures..."
	@$(MAKE) -C fall2025/lecture01 > /dev/null 2>&1 && echo "   ✓ Fall2025 lecture01 builds successfully" || echo "   ✗ Fall2025 lecture01 build failed"
	@$(MAKE) -C winter2025/lecture1 > /dev/null 2>&1 && echo "   ✓ Winter2025 lecture1 builds successfully" || echo "   ✗ Winter2025 lecture1 build failed"
	@echo ""
	@echo "Test build complete!"
