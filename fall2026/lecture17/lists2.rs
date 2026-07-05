struct List<T> {
    head: Link<T>,
}

type Link<T> = Option<Box<Node<T>>>;

struct Node<T> {
    elem: T,
    next: Link<T>,
}

impl<T> List<T> {
    fn new() -> Self {
        List { head: None }
    }

    fn push(&mut self, elem: T) {
        let new_node = Box::new(Node {
            elem,
            next: self.head.take(),  
        });
        self.head = Some(new_node);
    }

    fn pop(&mut self) -> Option<T> {
        match self.head.take() {
            Some(node) => {
                self.head = node.next;   // reattach the tail
                Some(node.elem)          // hand the element back by value
            }
            None => None,
        }
    }

    fn peek(&self) -> Option<&T> {
        match &self.head {
            Some(node) => Some(&node.elem),
            None => None,
        }
    }
}

fn main() {
    let mut list = List::new();
    list.push(1);
    list.push(2);
    list.push(3);                      // 3 -> 2 -> 1

    println!("{:?}", list.peek());     // Some(3)

    while let Some(x) = list.pop() {
        println!("popped {x}");        // 3, 2, 1
    }
    println!("{:?}", list.peek());     // None
}
