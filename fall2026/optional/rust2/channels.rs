use std::sync::mpsc;
use std::thread;
use std::time::Duration;

fn main() {
    let (sender, receiver) = mpsc::channel::<String>();

    let sender2 = sender.clone();

    let p1 = thread::spawn(move || {
        for i in 0..5 {
            sender.send(format!("producer-A says {i}")).unwrap();
            thread::sleep(Duration::from_millis(120));
        }
	// sender dropped here
    });

    let p2 = thread::spawn(move || {
        for i in 0..5 {
            sender2.send(format!("producer-B says {i}")).unwrap();
            thread::sleep(Duration::from_millis(200));
        }
        // 
    });

    // we can treat this as an iterator!
    for msg in receiver {
        println!("got: {msg}");
    }

    p1.join().unwrap();
    p2.join().unwrap();
}
