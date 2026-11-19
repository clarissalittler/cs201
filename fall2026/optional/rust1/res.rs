fn main() {
    let stuff : Result<String,()> = Ok("thingy".to_string());

    match stuff {
	Ok(p) => println!("{p}"),
	Err(_) => println!("There was an error!")
    }
}
