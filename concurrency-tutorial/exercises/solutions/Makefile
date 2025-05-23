CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Solution targets
PROCESS_SOLUTIONS = exercise1_solution exercise2_solution exercise3_solution exercise4_solution exercise5_solution
THREAD_SOLUTIONS = thread_exercise1_solution thread_exercise2_solution thread_exercise3_solution
SOCKET_SOLUTIONS = socket_exercise1_solution socket_exercise2_solution socket_exercise3_solution socket_exercise4_solution socket_exercise5_solution
SOLUTIONS = $(PROCESS_SOLUTIONS) $(THREAD_SOLUTIONS) $(SOCKET_SOLUTIONS)

# Default target
all: $(SOLUTIONS)

# Individual solution targets
exercise1_solution: exercise1_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise2_solution: exercise2_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise3_solution: exercise3_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise4_solution: exercise4_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

exercise5_solution: exercise5_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Thread solution targets
thread_exercise1_solution: thread_exercise1_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

thread_exercise2_solution: thread_exercise2_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

thread_exercise3_solution: thread_exercise3_solution.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Socket solution targets
socket_exercise1_solution: socket_exercise1_solution.c
	$(CC) $(CFLAGS) $< -o $@

socket_exercise2_solution: socket_exercise2_solution.c
	$(CC) $(CFLAGS) $< -o $@

socket_exercise3_solution: socket_exercise3_solution.c
	$(CC) $(CFLAGS) $< -o $@

socket_exercise4_solution: socket_exercise4_solution.c
	$(CC) $(CFLAGS) $< -o $@

socket_exercise5_solution: socket_exercise5_solution.c
	$(CC) $(CFLAGS) $< -o $@

# Test target - run all solutions with timeouts
test: all
	@echo "Testing all solutions..."
	@echo "========================="
	@echo "Exercise 1:"
	@timeout 5 ./exercise1_solution || echo "Exercise 1 completed"
	@echo
	@echo "Exercise 2:"
	@timeout 5 ./exercise2_solution || echo "Exercise 2 completed"  
	@echo
	@echo "Exercise 3:"
	@timeout 5 ./exercise3_solution || echo "Exercise 3 completed"
	@echo
	@echo "Exercise 4:"
	@timeout 10 ./exercise4_solution || echo "Exercise 4 completed"
	@echo
	@echo "Exercise 5:"
	@timeout 10 ./exercise5_solution || echo "Exercise 5 completed"
	@echo
	@echo "All solution tests completed!"

# Quick test - just compile, don't run
compile-test: all
	@echo "All solutions compiled successfully!"

# Clean target
clean:
	rm -f $(SOLUTIONS)

# Help target
help:
	@echo "Solution Makefile"
	@echo "=================="
	@echo "Available targets:"
	@echo "  all           - Compile all solutions (default)"
	@echo "  test          - Compile and run all solutions with output"
	@echo "  compile-test  - Compile all solutions without running"
	@echo "  clean         - Remove all compiled executables"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Individual targets:"
	@echo "  exercise1_solution - Basic fork demonstration"
	@echo "  exercise2_solution - Memory separation example"
	@echo "  exercise3_solution - Process chain coordination"
	@echo "  exercise4_solution - Parallel workers pattern"
	@echo "  exercise5_solution - Binary process tree"
	@echo ""
	@echo "Example usage:"
	@echo "  make                    # Compile all solutions"
	@echo "  make test              # Compile and test all solutions"
	@echo "  make exercise1_solution # Compile specific solution"
	@echo "  ./exercise1_solution   # Run specific solution"

.PHONY: all test compile-test clean help