fn main(){
    let mut v = vec![];
    println!("{:?}",v.pop());
    let x : i32 = v.pop().unwrap();
    println!("{x}");
}
