# Introduction to Rust Programming

This directory contains introductory examples for learning Rust, a systems programming language that emphasizes safety, concurrency, and performance.

## Contents

### Basic Programs
- **hello.rs** - Classic "Hello, World!" program
- **bigger.rs** - Larger example demonstrating more Rust features

### Input/Output Examples
- **echo0.rs** - Basic user input and output
- **echo1.rs** - Improved echo program
- **echo2.rs** - Further refinements
- **echofinal.rs** - Polished final version

### Data Structure Examples
- **petDatabase.rs** - Working with structured data
- **petDatabase2.rs** - Enhanced pet database

## Prerequisites

### Installing Rust

**Linux/macOS:**
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source $HOME/.cargo/env
```

**Verify installation:**
```bash
rustc --version
cargo --version
```

## Compilation and Running

### Compile and run individual programs:

```bash
# Compile
rustc hello.rs

# Run
./hello
```

### Using Cargo (Rust's build system and package manager):

```bash
# Create a new project
cargo new my_project
cd my_project

# Build and run
cargo run

# Build only (optimized)
cargo build --release
```

### Compile all examples:

```bash
# Compile each example
rustc hello.rs
rustc echo0.rs
rustc echo1.rs
rustc echo2.rs
rustc echofinal.rs
rustc bigger.rs
rustc petDatabase.rs
rustc petDatabase2.rs
```

## Running the Examples

### Hello World (hello.rs)

```bash
rustc hello.rs && ./hello
```

**Expected output:**
```
Hello, world!
```

### Echo Programs (echo0.rs - echofinal.rs)

Progressive examples showing input handling:

```bash
rustc echo0.rs && ./echo0
```

**Expected behavior:**
- Program prompts for input
- You type something and press Enter
- Program echoes back your input

**Try the progressively improved versions:**
```bash
rustc echo1.rs && ./echo1
rustc echo2.rs && ./echo2
rustc echofinal.rs && ./echofinal
```

### Pet Database (petDatabase.rs, petDatabase2.rs)

Examples of working with structs and data:

```bash
rustc petDatabase.rs && ./petDatabase
rustc petDatabase2.rs && ./petDatabase2
```

## Key Rust Concepts Demonstrated

### 1. Printing to Console
```rust
println!("Hello, world!");           // Print with newline
print!("No newline");                 // Print without newline
println!("Value: {}", variable);      // Print with formatting
```

### 2. Variables and Mutability
```rust
let x = 5;           // Immutable by default
let mut y = 10;      // Mutable variable
y = 15;              // OK: y is mutable
// x = 6;            // ERROR: x is immutable
```

### 3. User Input
```rust
use std::io;

let mut input = String::new();
io::stdin()
    .read_line(&mut input)
    .expect("Failed to read line");
```

### 4. String Handling
```rust
let s = String::new();           // Create new empty String
let s = String::from("hello");   // Create from literal
let trimmed = input.trim();      // Remove whitespace
```

### 5. Structs
```rust
struct Pet {
    name: String,
    species: String,
    age: u32,
}

let my_pet = Pet {
    name: String::from("Fluffy"),
    species: String::from("Cat"),
    age: 3,
};
```

## Rust vs C: Key Differences

### Memory Safety
- **Rust**: Memory safety guaranteed at compile time
- **C**: Manual memory management, potential for errors

### Ownership System
Rust's unique feature preventing data races and memory leaks:
```rust
let s1 = String::from("hello");
let s2 = s1;  // s1 is now invalid (moved to s2)
// println!("{}", s1);  // ERROR: s1 moved
```

### No Null Pointers
Rust uses `Option<T>` instead:
```rust
let some_number: Option<i32> = Some(5);
let absent_number: Option<i32> = None;
```

### Pattern Matching
```rust
match some_number {
    Some(x) => println!("Value: {}", x),
    None => println!("No value"),
}
```

### Error Handling
```rust
// Result type for operations that can fail
use std::fs::File;

let f = File::open("file.txt");
let f = match f {
    Ok(file) => file,
    Err(error) => panic!("Error: {:?}", error),
};
```

## Common Build Commands

### Basic compilation:
```bash
rustc filename.rs              # Compile source file
./filename                     # Run executable
```

### With optimization:
```bash
rustc -O filename.rs           # Optimize for speed
rustc -C opt-level=3 filename.rs  # Maximum optimization
```

### With debugging info:
```bash
rustc -g filename.rs           # Include debug symbols
```

### Check syntax without building:
```bash
rustc --check filename.rs      # Syntax check only
```

## Cargo Commands (Recommended)

```bash
cargo new project_name         # Create new project
cargo build                    # Build project
cargo run                      # Build and run
cargo test                     # Run tests
cargo check                    # Fast syntax check
cargo build --release          # Optimized build
cargo clean                    # Remove build artifacts
```

## Troubleshooting

### rustc: command not found
- Rust is not installed or not in PATH
- Run: `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
- Then: `source $HOME/.cargo/env`

### Compilation errors
- Rust has strict compile-time checks
- Read error messages carefully - they're usually very helpful
- The Rust compiler suggests fixes for many common errors

### Ownership errors
- Understanding ownership is key to Rust
- Use references (`&`) for borrowing instead of moving
- Use `.clone()` when you need a copy

## Learning Resources

### Official Documentation
- [The Rust Programming Language](https://doc.rust-lang.org/book/) - The official book
- [Rust By Example](https://doc.rust-lang.org/rust-by-example/) - Learn by examples
- [Rust Standard Library](https://doc.rust-lang.org/std/) - API documentation

### Online Tools
- [Rust Playground](https://play.rust-lang.org/) - Run Rust code in browser
- [Rustlings](https://github.com/rust-lang/rustlings) - Interactive exercises

## Why Rust for Systems Programming?

### Advantages
1. **Memory safety** without garbage collection
2. **Zero-cost abstractions** - high-level features with no runtime overhead
3. **Concurrency safety** - prevents data races at compile time
4. **Modern tooling** - Cargo, rustfmt, clippy
5. **Growing ecosystem** - Many libraries available via crates.io

### Use Cases
- Operating systems
- Embedded systems
- Web servers (high performance)
- Command-line tools
- Game engines
- Blockchain systems

## Comparison with C

| Feature | C | Rust |
|---------|---|------|
| Memory Safety | Manual | Automatic (ownership) |
| Null Pointers | Yes (dangerous) | No (Option type) |
| Data Races | Possible | Prevented at compile time |
| Package Manager | None built-in | Cargo |
| Error Messages | Basic | Detailed and helpful |
| Learning Curve | Moderate | Steep (ownership system) |

## Notes

- Rust emphasizes safety and correctness at compile time
- The borrow checker ensures memory safety without garbage collection
- Compilation is slower than C, but runtime performance is comparable
- Error messages in Rust are exceptionally helpful for learning
- These examples are introductory - Rust has many more advanced features
- For systems programming course, Rust complements C by showing alternative approaches

## See Also

- Main repository README for C programming examples
- `concurrency-tutorial/` for C concurrency examples (compare with Rust's approach)
- [Rust vs C comparison guide](https://doc.rust-lang.org/nomicon/)
