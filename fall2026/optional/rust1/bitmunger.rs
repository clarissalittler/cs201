// "bitmunger" -- the interactive bit-flipper from Lecture 1, in Rust.
//
// The loop is the same: show the number, show its bits, let the user
// flip a bit, optionally two's-complement it, repeat until they bail.
// What changes is what Rust lets us say *about* the number:
//   - i32 / u32 are different types; converting takes an explicit `as`
//   - bit-flipping uses the same ^/<< operators you know from C
//   - .wrapping_neg() / !x give us two's complement without the
//     "did this signed overflow into UB?" worry
//   - {n:032b} prints all 32 bits, padded with zeros, for free

use std::io;
use std::io::Write;

fn main() {
    let mut num: i32 = 4;

    loop {
        println!();
        println!("value (signed)   : {num}");
        println!("value (unsigned) : {}", num as u32);
        print_bits(num);

        let bit = prompt_i32("Enter a bit to flip (0-31, or -1 to quit): ");
        if bit == -1 { break; }
        if !(0..32).contains(&bit) {
            println!("  bit out of range, ignoring");
            continue;
        }
        num ^= 1i32 << bit;     // same trick as the C version

        let invert = prompt_i32("Invert? (1 = yes, 0 = no): ");
        if invert == 1 {
            num = twos_complement(num);
        }
    }
}

fn print_bits(n: i32) {
    // {:032b} pads the binary representation out to 32 chars with 0s
    // we cast to u32 first because Rust's binary formatter on signed
    // ints would happily print a leading '-' instead of the bit pattern
    println!("bits             : {:032b}", n as u32);
}

fn twos_complement(n: i32) -> i32 {
    // ~(x - 1) is the C trick from the original; in Rust !x is bitwise
    // NOT (not logical), and wrapping_sub keeps us from panicking on
    // INT_MIN in debug builds. Equivalent to n.wrapping_neg().
    !n.wrapping_sub(1)
}

fn prompt_i32(msg: &str) -> i32 {
    print!("{msg}");
    io::stdout().flush().unwrap();
    let mut buf = String::new();
    io::stdin().read_line(&mut buf).expect("read_line failed");
    buf.trim().parse().expect("not a number")
}
