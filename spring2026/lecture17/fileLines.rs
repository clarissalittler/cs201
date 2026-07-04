use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};

fn main() -> io::Result<()> {
    let path = env::args().nth(1).expect("usage: fileLines <file>");

    let file = File::open(&path)?;
    let reader = BufReader::new(file);

    for (i, line) in reader.lines().enumerate() {
        let line = line?; // we shadow the name line by deconstructing the line that has a Result type                     
        println!("{:4}: {}", i + 1, line);
    }

    Ok(())
}
