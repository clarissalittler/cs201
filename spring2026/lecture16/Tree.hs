data Tree a = Leaf
            | Branch (Tree a) (Tree a)

data Result a b = Ok a | Err b

data Option a = Some a | None
