use std::io;

struct Pet {
    name : String,
    species : String,
    age : u8,
}

fn main(){
    let mut pets = Vec::new();
    println!("It's time to tell us about some pets you've had");
    loop {
        println!("What was their name?");
        let mut input = String::new();
        io::stdin().read_line(&mut input).expect("Reading stdin failed");
        let name : String = input.trim().to_string();
        println!("What kind of animal are they?");
        input = String::new();
        io::stdin().read_line(&mut input).expect("Reading stdin failed");
        let species : String = input.trim().to_string();
        println!("How old are they?");
        input = String::new();
        io::stdin().read_line(&mut input).expect("Reading stdin failed");
        let age : u8 = input.trim().parse().expect("Couldn't parse as a number");
        let new_pet : Pet = Pet {name: name, species: species, age: age};
        pets.push(new_pet);

        input = String::new();
        println!("Do you wish to continue?");
        io::stdin().read_line(&mut input).expect("Reading stdin failed");
        if input.trim().eq_ignore_ascii_case("no"){
            break;
        }
    }

    for p in pets {
        println!("{} is a {} and is {} years old",p.name,p.species,p.age);
    }
}
