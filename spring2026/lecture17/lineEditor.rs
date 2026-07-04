// Lecture 13's "tiny terrible line editor" -- ported to Rust.

use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader, Write};

fn main() -> io::Result<()> {
    let path = env::args().nth(1).expect("usage: lineEditor <file>");

        let lines: Vec<String> = BufReader::new(File::open(&path)?)
        .lines()
        .collect::<io::Result<Vec<_>>>()?;

    let mut lines = lines; // this is a common idiom in Rust, to shadow the variable as a mutable version once youo're going to change it

    loop {
        show(&lines);
        match prompt("Command? (e)dit (i)nsert (d)elete (q)uit: ")?.as_str() {
            "e" => edit(&mut lines)?,
            "i" => insert(&mut lines)?,
            "d" => delete(&mut lines)?,
            "q" => break,
            other => println!("don't know what '{other}' means"),
        }
    }

    // truncate and rewrite. File::create truncates; writeln! adds the \n
    let mut out = File::create(&path)?;
    for line in &lines {
        writeln!(out, "{line}")?;
    }
    Ok(())
}

fn show(lines: &[String]) {
    println!("---");
    for (i, l) in lines.iter().enumerate() {
        println!("{i:3}: {l}");
    }
    println!("---");
}

fn edit(lines: &mut Vec<String>) -> io::Result<()> {
    let i = prompt("line to edit: ")?.parse().unwrap_or(usize::MAX);
    if i >= lines.len() {
	println!("out of range");
	return Ok(());
    }
    lines[i] = prompt("new contents: ")?;
    Ok(())
}

fn insert(lines: &mut Vec<String>) -> io::Result<()> {
    let i = prompt("insert before line: ")?.parse().unwrap_or(lines.len());
    let s = prompt("new contents: ")?;
    let i = i.min(lines.len());
    lines.insert(i, s);  
    Ok(())
}

fn delete(lines: &mut Vec<String>) -> io::Result<()> {
    let i = prompt("line to delete: ")?.parse().unwrap_or(usize::MAX);
    if i >= lines.len() {
	println!("out of range");
	return Ok(()); }
    lines.remove(i);     
    Ok(())
}

fn prompt(msg: &str) -> io::Result<String> {
    print!("{msg}");
    io::stdout().flush()?;
    let mut s = String::new();
    io::stdin().read_line(&mut s)?;
    Ok(s.trim().to_string())
}
