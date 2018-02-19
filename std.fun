#logic
a b <= ? a b < : 1 ? a b = : 1 ? 0
a b >= ? a b > : 1 ? a b = : 1 ? 0

#arithmetic
a is_even : a 2 % ~
a b is_div : b a % ~
a dup : a a
a b first : a
a b second : b
a b min ? a b < : a ? b
a b max ? a b > : a ? b
base exp ^ ? exp 1 = : base ? base base exp 1 - ^ *
a ! ? a 1 = : 1 ? a a 1 - ! *

#stacks
a n elem_at ? a is_empty : ? n 1 = : a pop ? a popped n 1 - elem
a flatten ? a is_empty : ? a pop a popped flatten
a f map ? a is_empty : ( ) ? a popped f map a pop f flatten push
a size ? a is_empty : 0 ? 1 a popped size +
a last ? a size 1 = : a pop ? a popped last
n i range_back ? i n = : ( ) ? n i 1 + range_back i push
n range : n 0 range_back
a b i elem_back ? a is_empty : -1 ? a pop b = : i ? a popped b i 1 + elem_back
a b elem : a b 0 elem_back
a b reverse_back ? a is_empty : b ? a popped b a pop push reverse_back
a reverse : a ( ) reverse_back
a b zip ? a is_empty b is_empty | : ( ) ? a popped b popped zip ( ) b pop push a pop push push
