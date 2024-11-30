use std::io;

fn main() {
    let mut input = String::new();
    println!("Enter something, will ya?");
    io::stdin()
        .read_line(&mut input).expect("Reading stdin failed!");
    println!("You said: {}",input.trim());
}
