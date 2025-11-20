# IPC Examples - Robust Versions

This directory contains **production-ready** versions of IPC examples with comprehensive error handling, resource management, and defensive programming practices.

## Purpose

These robust versions demonstrate:
- **Complete error checking** on all system calls
- **Proper resource cleanup** even on error paths
- **Defensive programming** against edge cases
- **Production-quality** code practices
- **Clear error messages** with actionable information

## Comparison with Minimal Versions

| Aspect | Minimal Version (../05-ipc/) | Robust Version (this directory) |
|--------|------------------------------|--------------------------------|
| **Purpose** | Educational clarity | Production readiness |
| **Error Checking** | Minimal/illustrative | Comprehensive |
| **Resource Cleanup** | Basic | Guaranteed on all paths |
| **Error Messages** | Simple | Detailed with errno |
| **Edge Cases** | Not handled | Handled defensively |
| **Code Length** | ~200 lines | ~400 lines |
| **Use Case** | Learning concepts | Real applications |

## Contents

### Currently Available

1. **01-pipes-robust.c** - Anonymous pipes with full error handling
   - Comprehensive error checking on all pipe operations
   - Proper cleanup on all exit paths
   - Handles partial reads/writes
   - Verifies child exit status

2. **04-socket-ipc-robust.c** - Unix domain sockets with production-quality code
   - Complete socket error handling
   - Buffer overflow protection
   - Graceful connection handling
   - Resource cleanup guarantees

### Planned

3. **02-named-pipes-robust.c** - Named pipes with robust error handling
4. **03-shared-memory-robust.c** - Shared memory with complete synchronization

## Compilation

```bash
# Compile individual examples
gcc -Wall -Wextra -std=c11 -o 01-pipes-robust 01-pipes-robust.c
gcc -Wall -Wextra -std=c11 -o 04-socket-ipc-robust 04-socket-ipc-robust.c

# Or using a Makefile (if created)
make
```

## Running the Examples

Usage is identical to the minimal versions, but with better error reporting:

### 01-pipes-robust
```bash
./01-pipes-robust
```

### 04-socket-ipc-robust

**Terminal 1 (Server):**
```bash
./04-socket-ipc-robust server
```

**Terminal 2 (Client):**
```bash
./04-socket-ipc-robust client
```

## Key Improvements Over Minimal Versions

### 1. Error Checking
**Minimal:**
```c
pipe(pipe_fd);  // No error check
```

**Robust:**
```c
if (pipe(pipe_fd) == -1) {
    fprintf(stderr, "ERROR: Failed to create pipe: %s\n", strerror(errno));
    return EXIT_FAILURE;
}
```

### 2. Resource Cleanup
**Minimal:**
```c
close(fd);  // May leak if function returns early
```

**Robust:**
```c
// Cleanup on all paths, including error paths
if (close(fd) == -1) {
    fprintf(stderr, "ERROR: Failed to close fd: %s\n", strerror(errno));
    ret = EXIT_FAILURE;
}
// Continue with other cleanup even if one fails
```

### 3. Buffer Safety
**Minimal:**
```c
strcpy(addr.sun_path, path);  // Buffer overflow risk
```

**Robust:**
```c
if (strlen(path) >= sizeof(addr.sun_path)) {
    fprintf(stderr, "ERROR: Path too long\n");
    return EXIT_FAILURE;
}
strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
```

### 4. Partial Operations
**Minimal:**
```c
write(fd, buf, len);  // Assumes all data written
```

**Robust:**
```c
ssize_t written = write(fd, buf, len);
if (written == -1) {
    fprintf(stderr, "ERROR: Write failed: %s\n", strerror(errno));
    return EXIT_FAILURE;
}
if ((size_t)written != len) {
    fprintf(stderr, "WARNING: Partial write (%zd/%zu bytes)\n",
            written, len);
}
```

### 5. Error Messages
**Minimal:**
```c
perror("error");  // Generic message
```

**Robust:**
```c
fprintf(stderr, "Server ERROR: Failed to bind socket: %s\n", strerror(errno));
fprintf(stderr, "Server: Is another process using this socket?\n");
```

## When to Use Each Version

### Use Minimal Versions When:
- Learning IPC concepts for the first time
- Creating quick prototypes
- Writing documentation examples
- Teaching where error handling would obscure the concept
- Code clarity is more important than robustness

### Use Robust Versions When:
- Writing production code
- Building reliable systems
- Learning proper error handling
- Debugging complex IPC issues
- Code robustness is critical
- Need to handle edge cases

## Learning Path

1. **Start with minimal versions** (../05-ipc/) to understand IPC concepts
2. **Read the concurrency guide** (../../concurrencyGuide.org) for theory
3. **Study robust versions** (this directory) to learn error handling
4. **Compare side-by-side** to see what production code adds
5. **Practice both styles** depending on your use case

## Code Quality Checklist

These robust examples demonstrate:

- ✅ Check return value of every system call
- ✅ Use strerror(errno) for detailed error messages
- ✅ Close all file descriptors on all exit paths
- ✅ Validate all buffer operations
- ✅ Handle partial reads/writes
- ✅ Null-terminate all strings
- ✅ Avoid buffer overflows (use snprintf, strncpy)
- ✅ Provide actionable error messages
- ✅ Distinguish errors from warnings
- ✅ Clean up even when errors occur
- ✅ Return proper exit codes
- ✅ Initialize all variables
- ✅ Check for edge cases (EOF, disconnection, etc.)

## Further Reading

- **Minimal versions:** See `../05-ipc/README.md` for concept-focused examples
- **Theory:** See `../../concurrencyGuide.org` for IPC fundamentals
- **Error handling:** `man errno`, `man strerror`
- **Best practices:** "The Art of Unix Programming" by Eric S. Raymond
