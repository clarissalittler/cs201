fn main(){
    let mut v : Vec<i32> = vec![];
    println!("{:?}",v.pop());
    match v.pop() {
	Some(x) => println!("{x}"),
	None => println!("What have you done!")
    }
}
