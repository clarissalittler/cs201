fn helper(v : &Vec<i32>) -> u32 {
    let mut c = 0;
    for _i in v {
	c += 1;
    }
    c
}

fn main() {
    let v = vec![1,2,3,4];
    println!("{}",helper(&v));

    for x in v {
	println!("{x}");
    }
}
