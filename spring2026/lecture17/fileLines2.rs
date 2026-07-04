use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};

fn main() -> io::Result<()> {
    let path = env::args().nth(1).expect("usage: fileLines <file>");

    let file = File::open(&path)?;
    let reader = BufReader::new(file);

    for (i, line) in reader.lines().enumerate() {
	match line {
	    Ok(l) => println!("{:4}: {}", i + 1, l),
	    Err(e) => return Err(e)
	}
    }

    Ok(())
}
