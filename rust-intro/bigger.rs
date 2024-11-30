use std::io;
use std::cmp::Ordering;

fn main(){
    let mut input = String::new();
    println!("Enter a number: ");
    io::stdin()
        .read_line(&mut input).expect("Reading stdin failed");
    let num1 : i32 = input.trim().parse().expect("Couldn't parse as a number");
    input.clear();
    println!("Enter another number: ");
    io::stdin()
        .read_line(&mut input).expect("Reading stdin failed");
    let num2 = input.trim().parse().expect("Couldn't parse as a number");
    match num1.cmp(&num2) {
        Ordering::Greater => println!("The first number was bigger"),
        Ordering::Less => println!("The second number was bigger"),
        Ordering::Equal => println!("The two numbers are equal")
    }
}
