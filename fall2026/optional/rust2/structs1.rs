use std::fmt;

#[derive(Debug, Clone, Copy, PartialEq)]
//#[derive(Debug, PartialEq)]
struct Vec2 {
    x: f64,
    y: f64
}

impl fmt::Display for Vec2 {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "({}, {})", self.x, self.y)
    }
}

impl Vec2 {
    fn origin() -> Vec2 {
	Vec2{x : 0.0, y: 0.0}
    }
    fn scale(&mut self, c : f64) {
	self.x = c*self.x;
	self.y = c*self.y;
    }

    fn norm(&self) -> f64 {
	(self.x * self.x + self.y*self.y).sqrt()
    }

    fn add(&self, v : Vec2) -> Vec2 {
	Vec2{x: self.x + v.x, y: self.y+v.y}
    }
    fn dot(&self, v : Vec2) -> f64 {
	self.x * v.x + self.y * v.y
    }
}

fn main(){

    let v1 = Vec2::origin();
    let v2 = Vec2{x: 3.0, y: 4.0};
    let mut v3 = Vec2{x: 4.0, y: -3.0};

    println!("{}",v1.add(v2));
    println!("{}",v2.dot(v3));
    v3.scale(4.0);
    println!("{}",v3.dot(v2));
}
