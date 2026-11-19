fn main(){
    let mut v = vec![1,2,3,4];
    for x in &mut v {
	*x += 1;
	println!("{:?}",*x);
    }
    for x in v {
	println!("{x}");
    }
}
