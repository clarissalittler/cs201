use std::io::{self, Write};

#[derive(PartialEq, Debug)]
enum CSClasses {
    CS250,
    CS201,
    CS161,
    CS162,
    CS251,
    CS260,
}

impl CSClasses {
    // The safe "read into the enum" step: every path returns a value,
    // and anything we don't recognize becomes None instead of garbage.
    fn from_number(n: u32) -> Option<CSClasses> {
        match n {
            250 => Some(CSClasses::CS250),
            201 => Some(CSClasses::CS201),
            161 => Some(CSClasses::CS161),
            162 => Some(CSClasses::CS162),
            251 => Some(CSClasses::CS251),
            260 => Some(CSClasses::CS260),
            _   => None,
        }
    }
}

fn main() {
    print!("Enter a course number: ");
    io::stdout().flush().unwrap();

    let mut line = String::new();
    io::stdin().read_line(&mut line).unwrap();

    // .trim().parse() is itself fallible -> Result, so we handle both
    // "not even a number" and "a number that isn't one of our classes".
    match line.trim().parse::<u32>() {
        Ok(n) => match CSClasses::from_number(n) {
            Some(class) => println!("Got a real class: {:?}", class),
            None        => println!("{} isn't a CS class I know about.", n),
        },
        Err(_) => println!("That wasn't even a number!"),
    }
}
