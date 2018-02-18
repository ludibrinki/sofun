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
exp base ^ ? exp 1 < : 1 ? base exp 1 - base ^ *
a ! ? a 1 = : 1 ? a a 1 - ! *

#stacks
a n elem ? a is_empty : ? n 1 = : a pop ? a popped n 1 - elem
a flatten ? a is_empty : ? a pop a popped flatten
a f map ? a is_empty : ? a pop f flatten a popped f map
