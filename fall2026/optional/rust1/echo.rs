use std::io;

fn main() {
    let mut input = String::new(); // okay we need to talk about =mut=
    println!("Say something: ");
    io::stdin()
        .read_line(&mut input) 
        .expect("reading stdin failed"); // note that we can compose operations pretty easily

    // read_line keeps the trailing newline; .trim() strips it
    println!("You said: {}", input.trim());
}
