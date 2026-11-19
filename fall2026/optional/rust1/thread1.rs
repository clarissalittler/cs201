use std::thread;

fn main(){

    let mut handles = vec![];
    for _i in 0..10 {
	handles.push(thread::spawn( || {
	    println!("Puppy");
	}));
    }
    for h in handles { h.join().unwrap(); }
}
