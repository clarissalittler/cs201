fn main(){
    let v = vec![1,2,3,4];
    for x in &v {
	let mut y = *x;
	y += 1;
	println!("{y}");
    }
    for x in v {
	println!("{x}");
    }
}
