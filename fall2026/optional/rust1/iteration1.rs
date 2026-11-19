fn main(){
    let mut x = 0;
    loop {
	println!("Looping for the {x}th time");
	x += 1;
	if x > 10 {
	    break;
	}
    }
}
