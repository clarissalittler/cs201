use std::thread;

fn main(){

    let mut handles = vec![];
    for i in 0..10 {
	handles.push(thread::spawn( move || {
	    println!("thread {i}");
	}));
    }
    for h in handles { h.join().unwrap(); }
}
