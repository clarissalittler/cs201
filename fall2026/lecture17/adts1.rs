#[derive(PartialEq)]
enum Thingy {
    AThing(i32),
    Another(f64,f64),
}

fn main(){
    let t = Thingy::AThing(5);
    match t {
	Thingy::AThing(i) => println!("{i}"),
	Thingy::Another(f1,f2) => println!("{f1},{f2}")
    }
}

