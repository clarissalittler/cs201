enum List<T> {
    Cons(T, Box<List<T>>),
    Nil,
}

// vector<int>

impl<T> List<T> {
    fn new() -> Self {
        List::Nil
    }

    fn prepend(self, elem: T) -> Self {
        List::Cons(elem, Box::new(self))
    }

    fn len(&self) -> usize {
        match self {
            List::Cons(_, next) => 1 + next.len(),
            List::Nil => 0,
        }
    }

    fn head(&self) -> Option<&T> {
        match self {
            List::Cons(elem, _) => Some(elem),
            List::Nil => None,
        }
    }
}

fn main() {
    let list = List::new().prepend(3).prepend(2).prepend(1); // 1 -> 2 -> 3
    println!("len = {}", list.len());        // 3
    match list.head() {
        Some(x) => println!("head = {x}"),   // 1
        None => println!("empty"),
    }
}
