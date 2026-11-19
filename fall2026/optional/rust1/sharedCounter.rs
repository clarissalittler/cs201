use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
    /* ---- BROKEN VERSION: does not compile ----
    let mut counter = 0i32;
    let t1 = thread::spawn(|| {
        for _ in 0..100_000 { counter += 1; }
    });
    let t2 = thread::spawn(|| {
        for _ in 0..100_000 { counter += 1; }
    });
    t1.join().unwrap();
    t2.join().unwrap();
    println!("counter = {counter}"); */

    let counter = Arc::new(Mutex::new(0i32));

    let mut handles = Vec::new();
    for _ in 0..2 {
        let c = Arc::clone(&counter);   // bump the refcount for the new thread
        handles.push(thread::spawn(move || {
            for _ in 0..100_000 {
                let mut guard = c.lock().unwrap(); // *guard is the i32
                *guard += 1;
                // guard is dropped here -> mutex released. no manual unlock.
            }
        }));
    }

    for h in handles { h.join().unwrap(); }

    println!("final counter (expected 200000): {}", *counter.lock().unwrap());
}
