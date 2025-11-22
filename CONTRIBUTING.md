# Contributing to CS201 Systems Programming Repository

Thank you for your interest in contributing to this educational repository! This guide will help you maintain consistency and quality across all materials.

## Table of Contents

- [Getting Started](#getting-started)
- [Repository Structure](#repository-structure)
- [Coding Standards](#coding-standards)
- [Documentation Standards](#documentation-standards)
- [File Naming Conventions](#file-naming-conventions)
- [Makefile Standards](#makefile-standards)
- [Adding New Content](#adding-new-content)
- [Testing Your Changes](#testing-your-changes)
- [Commit Guidelines](#commit-guidelines)

## Getting Started

1. **Understand the repository structure**: Read the main `README.md`
2. **Review existing examples**: Look at similar content before creating new materials
3. **Follow the patterns**: Consistency helps students navigate the repository
4. **Test your code**: Ensure all code compiles and runs correctly

## Repository Structure

The repository follows a clear organizational pattern:

```
cs201/
├── fall2024/           # Fall 2024 semester lectures
├── fall2025/           # Fall 2025 semester lectures
├── winter2025/         # Winter 2025 semester lectures
├── spring2025/         # Spring 2025 topic-based materials
├── concurrency-tutorial/  # Comprehensive concurrency guide
├── assembly-examples/     # Progressive assembly examples
├── ipc-lecture/          # IPC mechanisms
├── deadlock-lecture/     # Deadlock examples
├── file-intro/           # File I/O introduction
├── socket-intro/         # Socket programming intro
└── rust-intro/           # Rust language introduction
```

### When Adding New Content

- **Lecture materials**: Add to appropriate semester directory
- **Tutorial examples**: Add to relevant tutorial collection
- **Exercises**: Add to `concurrency-tutorial/exercises/`
- **Standalone topics**: Create new top-level directory with descriptive README

## Coding Standards

### C Code

**Style Guidelines:**
- Use 4-space indentation (no tabs for C code)
- Include error checking for system calls
- Add descriptive variable names
- Use comments to explain non-obvious logic
- Keep functions focused and modular

**Example:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Error handling
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process: PID = %d\n", getpid());
        exit(0);
    } else {
        // Parent process
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        wait(NULL);
    }

    return 0;
}
```

**Required Headers:**
- Always include necessary system headers
- Add comments explaining what each header provides
- Use standard library includes (`<>`) for system headers

**Educational Code:**
- Prioritize clarity over optimization
- Add inline comments for complex operations
- Show both simple and complex examples
- Demonstrate proper error handling

### Assembly Code

**Style Guidelines:**
- Use AT&T syntax (required for this course)
- Use tabs for indentation in assembly files
- Align comments at consistent column (typically column 40-50)
- Include section directives (`.text`, `.data`, `.bss`)
- Add function prologues/epilogues where appropriate

**Naming Patterns:**
- `filename.s` - Basic uncommented version
- `filenameGuided.s` - Version with moderate comments
- `filenamecommented.s` - Heavily annotated version (Deep-Dive)

**Example:**
```asm
.section .text
.globl _start

_start:
    movq    $1, %rax        # syscall: write
    movq    $1, %rdi        # file descriptor: stdout
    movq    $msg, %rsi      # buffer address
    movq    $len, %rdx      # buffer length
    syscall

    movq    $60, %rax       # syscall: exit
    movq    $0, %rdi        # exit code: 0
    syscall

.section .data
msg:    .ascii "Hello, World!\n"
len = . - msg
```

## Documentation Standards

### README Files

Every directory with code examples **must** have a `README.md` file.

**Template Structure** (based on `winter2025/lecture13/README.md`):

```markdown
# [Directory Title] - [Topic Name]

Brief description of what this directory contains.

## Contents

### [Subsection Name]
- **filename.c** - Brief description
- **filename2.c** - Brief description

## Compilation

### Compile all examples:
\`\`\`bash
make
\`\`\`

### Compile specific examples:
\`\`\`bash
gcc -o program program.c
\`\`\`

## Running the Examples

### [Example Name]

**Terminal 1:**
\`\`\`bash
./program1
\`\`\`

**Expected behavior:**
- Description of what should happen
- Key observations

## Key Concepts Demonstrated

- **Concept 1**: Explanation
- **Concept 2**: Explanation

## Troubleshooting

### Issue: Description
\`\`\`bash
# Solution commands
\`\`\`

## Notes

- Important reminders
- Common pitfalls
- Best practices
```

**README Best Practices:**
- Start with clear title indicating topic
- List all source files with descriptions
- Provide compilation instructions
- Include running instructions for multi-process examples
- Explain expected behavior
- Add troubleshooting section for common issues
- Include notes on prerequisites and related topics

### Agenda Files

Lecture directories should contain an `Agenda.org` file (capital A):

```org
* CS201 - Lecture [Number]: [Topic]
** Learning Objectives
- Objective 1
- Objective 2

** Topics Covered
- Topic 1
- Topic 2

** Examples
- filename.c: Description
- filename2.c: Description
```

## File Naming Conventions

### Consistency is Critical

**C Source Files:**
- Descriptive lowercase names: `fork1.c`, `thread_example.c`
- Commented versions: `fork1commented.c`, `thread_examplecommented.c`
- Exercise files: `exercise1.c`, `exercise2.c`
- Solution files: `exercise1_solution.c`, `exercise2_solution.c`

**Assembly Files:**
- Basic version: `program.s`
- Guided version: `programGuided.s`
- Deep-dive version: `programcommented.s`

**Headers:**
- Lowercase with underscores: `shared_data.h`
- Include guards using uppercase: `SHARED_DATA_H`

**Executables:**
- Same name as source file (no extension)
- Should be in `.gitignore` (executables are not committed)

**Documentation:**
- Main README: `README.md` (all caps)
- Agenda files: `Agenda.org` (capital A)
- Contributing guide: `CONTRIBUTING.md`

## Makefile Standards

Every directory with compilable code should have a `Makefile`.

### For C Programs (Lecture Directories)

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Find all C source files (excluding commented versions)
C_SOURCES = $(filter-out %commented.c, $(wildcard *.c))
EXECUTABLES = $(C_SOURCES:.c=)

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(EXECUTABLES)

.PHONY: all clean
```

### For Assembly Programs

```makefile
.SUFFIXES:

ASM_FILES := $(wildcard *.s)
OBJ_FILES := $(ASM_FILES:.s=.o)
EXECUTABLES := $(ASM_FILES:.s=)

all: $(EXECUTABLES)

%: %.o
	ld $< -o $@

%.o: %.s
	as $< -o $@

clean:
	rm -f $(OBJ_FILES) $(EXECUTABLES)

.PHONY: all clean
```

### For Exercise Directories

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

EXERCISES = exercise1 exercise2 exercise3 exercise4 exercise5

all: $(EXERCISES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(EXERCISES)

test: all
	@echo "Run individual exercises to test"

.PHONY: all clean test
```

**Makefile Best Practices:**
- Always include `all` and `clean` targets
- Use `.PHONY` for non-file targets
- Add `-Wall -Wextra` for comprehensive warnings
- Include `-g` for debugging symbols
- Use `-pthread` for threading, `-lrt` for POSIX IPC
- Filter out commented versions from compilation

## Adding New Content

### Adding a New Lecture

1. **Create directory**: `mkdir semester/lectureXX`
2. **Add Agenda.org**: Outline topics and objectives
3. **Add source files**: Include both original and commented versions where helpful
4. **Create README.md**: Following the template structure
5. **Create Makefile**: Following the standards above
6. **Test**: Ensure everything compiles with `make`
7. **Update semester README**: Add lecture to the semester's README.md

### Adding a New Exercise Set

1. **Create directory**: `concurrency-tutorial/exercises/0X-topic-name/`
2. **Create README.md**: Include:
   - Learning objectives
   - Exercise descriptions with difficulty levels (🟢🟡🔴)
   - Compilation and testing instructions
   - Tips and prerequisites
3. **Create exercise files**: `exercise1.c` through `exerciseN.c` with TODO comments
4. **Create solutions**: In `concurrency-tutorial/exercises/solutions/0X-topic-name/`
5. **Create Makefile**: Following exercise Makefile pattern
6. **Update exercises README**: Add to `concurrency-tutorial/exercises/README.md`
7. **Test**: Verify all exercises and solutions compile and run

### Adding a New Tutorial

1. **Create directory**: `topic-tutorial/` or `topic-examples/`
2. **Create comprehensive README.md**: Include:
   - Overview and learning objectives
   - Directory structure
   - Build instructions
   - Learning path recommendations
3. **Organize examples**: By topic or difficulty
4. **Create Makefile**: Following tutorial Makefile pattern
5. **Add to main README**: Update repository's main README.md
6. **Add to top-level Makefile**: Add build target if appropriate

## Testing Your Changes

### Before Committing

1. **Compile all code**:
   ```bash
   make clean
   make
   ```

2. **Test executables**:
   ```bash
   ./program1
   ./program2
   # Test with expected inputs/scenarios
   ```

3. **Check for warnings**:
   ```bash
   gcc -Wall -Wextra -Wpedantic program.c -o program
   ```

4. **Verify documentation**:
   - Ensure README is clear and complete
   - Check that all files are documented
   - Verify compilation instructions work

5. **Test from clean state**:
   ```bash
   make clean
   make all
   # Verify everything builds successfully
   ```

### Using the Top-Level Makefile

The repository includes a top-level Makefile for testing:

```bash
# Test major components
make test

# Build all tutorials
make tutorials

# Build specific semester
make fall2025

# Build all exercises
make exercises
```

## Commit Guidelines

### Commit Messages

Write clear, descriptive commit messages:

**Good examples:**
```
Add lecture 9 README with threading concepts

Create synchronization exercises with 5 progressive examples

Fix race condition in thread_example.c

Update fall2025 README with new lecture topics
```

**Poor examples:**
```
update
fix stuff
changes
wip
```

### Commit Structure

**Format:**
```
[Type] Brief description (50 chars or less)

More detailed explanation if needed (wrap at 72 characters).
Explain what and why, not how (the code shows how).

- Bullet points for multiple changes
- Keep related changes together
- Reference issue numbers if applicable
```

**Types:**
- `Add`: New files, features, or content
- `Update`: Modifications to existing content
- `Fix`: Bug fixes or corrections
- `Refactor`: Code restructuring without functionality change
- `Docs`: Documentation-only changes
- `Build`: Makefile or build system changes

### What to Commit

**DO commit:**
- Source code (`.c`, `.s`, `.h`, `.rs` files)
- Documentation (`.md`, `.org` files)
- Build files (`Makefile`)
- Configuration files
- README files

**DO NOT commit:**
- Executables (compiled binaries)
- Object files (`.o`)
- Temporary files (`*~`, `.swp`)
- IDE-specific files (`.vscode/`, `.idea/`)
- OS-specific files (`.DS_Store`, `Thumbs.db`)
- Large binary files

Ensure `.gitignore` is properly configured.

## Code Review Checklist

Before submitting changes, verify:

- [ ] All code compiles without errors or warnings
- [ ] All programs run correctly
- [ ] README.md exists and is complete
- [ ] Makefile exists and works (if applicable)
- [ ] File naming follows conventions
- [ ] Code includes appropriate comments
- [ ] Error handling is present
- [ ] Examples are educational and clear
- [ ] Documentation is accurate
- [ ] Changes are tested from clean state
- [ ] Commit messages are descriptive
- [ ] No compiled binaries or temporary files included

## Questions or Issues?

If you have questions about contributing:

1. Review existing examples in the repository
2. Check the main README.md for organizational guidance
3. Look at `winter2025/lecture13/` as a reference for quality documentation
4. Examine `concurrency-tutorial/` for comprehensive tutorial structure

## Educational Philosophy

Remember that this is an **educational repository**. When contributing:

- **Clarity over cleverness**: Write code that teaches, not code that impresses
- **Progressive complexity**: Start simple, build to advanced
- **Explain the "why"**: Comments should explain reasoning, not just actions
- **Practical examples**: Use realistic scenarios that students can relate to
- **Proper error handling**: Teach good habits, even in simple examples
- **Consistency**: Following patterns helps students focus on concepts, not navigation

Thank you for helping make CS201 a better learning resource!
