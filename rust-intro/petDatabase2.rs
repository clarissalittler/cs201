use std::io;
use std::io::Write;
use std::io::Result;

struct Pet {
    name : String,
    species : String,
    age : u8,
}

fn read_line(prompt: &str) -> Result<String> {
  print!("{prompt}");
  io::stdout().flush()?;
  let mut input = String::new();
  io::stdin().read_line(&mut input)?;
  Ok(input.trim().to_string())
}

fn main() -> Result<()>{
    let mut pets = Vec::new();
    println!("It's time to tell us about some pets you've had");
    loop {
        let name = read_line("What is their name?: ")?;
        let species = read_line("What kind of animal are they?: ")?;
        let age : u8 = read_line("How old are they?: ")?.parse().expect("Needed a number");
        let new_pet = Pet {name: name, species: species, age: age};
        pets.push(new_pet);

        let to_cont = read_line("Do you wish to continue?: ")?;
        if to_cont.eq_ignore_ascii_case("no"){
            break;
        }
    }

    for p in pets {
        println!("{} is a {} and is {} years old",p.name,p.species,p.age);
    }
    Ok(())
}
