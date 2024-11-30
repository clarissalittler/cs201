use std::io;

fn main() {
    let mut input = String::new();
    println!("Enter something, will ya?");
    io::stdin()
        .read_line(&mut input);
    println!("You said: {}",input.trim());
}
