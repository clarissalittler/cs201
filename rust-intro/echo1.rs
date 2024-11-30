use std::io;

fn main() {
    let mut input = String::new();
    println!("Enter something, will ya?");
    let res =io::stdin().read_line(&mut input);
    match res {
        Ok(bytes) => {
            println!("Okay so you read in {}",bytes);
            println!("When you said: {}",input.trim());
        }
        Err(e) => {
            println!("There was an error of: {}",e);
        }
    }
}
