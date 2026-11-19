
fn main() {
    let mut v = vec![1, 2, 3];
    let first = &v[0];
    println!("{first}");  // last use of `first`
    v.push(4);            // fine — the borrow ended at the line above
}
