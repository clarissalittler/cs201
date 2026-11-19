// the obligatory first program
// println! is a *macro* (note the !), not a function -- it does
// formatted printing at compile time, so the format string and
// the arguments are checked when you build, not when you run
fn main() {
    println!("Hello, world!");
}
