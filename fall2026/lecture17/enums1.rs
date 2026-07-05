#[derive(PartialEq)]
enum CSClasses {
    CS250,
    CS201,
    CS161,
    CS162,
    CS251,
    CS260
}

fn main(){
    println!("{}",CSClasses::CS250 == CSClasses::CS251);
    
}
