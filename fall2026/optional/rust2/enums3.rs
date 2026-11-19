use std::convert::TryFrom;
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

impl TryFrom<u32> for CSClasses {
    // The Error type is part of the contract: what do you get on failure?
    type Error = String;

    fn try_from(n: u32) -> Result<CSClasses, Self::Error> {
        match n {
            250 => Ok(CSClasses::CS250),
            201 => Ok(CSClasses::CS201),
            161 => Ok(CSClasses::CS161),
            162 => Ok(CSClasses::CS162),
            251 => Ok(CSClasses::CS251),
            260 => Ok(CSClasses::CS260),
            _   => Err(format!("{} isn't a CS class I know about.", n)),
        }
    }
}

fn main() {
    print!("Enter a course number: ");
    io::stdout().flush().unwrap();

    let mut line = String::new();
    io::stdin().read_line(&mut line).unwrap();

    // parse() hands us a Result; map_err lets us speak the same Result<_, String>
    // language as TryFrom, and then and_then chains the two fallible steps into one.
    let result = line
        .trim()
        .parse::<u32>()
        .map_err(|_| "That wasn't even a number!".to_string())
        .and_then(CSClasses::try_from);

    match result {
        Ok(class) => println!("Got a real class: {:?}", class),
        Err(msg)  => println!("{}", msg),
    }
}
