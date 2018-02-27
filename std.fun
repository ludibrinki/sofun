#logic
a b <= ? a b < : 1 ? a b = : 1 ? 0
a b >= ? a b > : 1 ? a b = : 1 ? 0
!= : = ~

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
a f e foldl ? a is_empty : e ? a popped f e a pop f flatten foldl 
a f map ? a is_empty : ( ) ? a popped f map a pop f flatten push
a f filter ? a is_empty : ( ) ? a pop f flatten : a popped f filter a pop push ? a popped f filter
a b eq_stack ? a size b size != : 0 ? a is_empty : 1 ? a pop b pop = a popped b popped eq_stack &  
a n elem_at ? a is_empty : ? n 1 = : a pop ? a popped n 1 - elem
a flatten ? a is_empty : ? a popped flatten a pop
a size ? a is_empty : 0 ? 1 a popped size +
a last ? a size 1 = : a pop ? a popped last
i n range ? i n = : ( ) ? i 1 + n range i push
a b i elem_back ? a is_empty : -1 ? a pop b = : i ? a popped b i 1 + elem_back
elem : 0 elem_back
a b reverse_back ? a is_empty : b ? a popped b a pop push reverse_back
reverse : ( ) reverse_back
a b zip ? a is_empty b is_empty | : ( ) ? a popped b popped zip ( ) b pop push a pop push push
a s e slice ? s 0 > : a popped s 1 - e 1 - slice ? e 0 <= : ( ) ? a popped s e 1 - slice a pop push
sum : ( + ) 0 foldl
a min_stack : a popped ( min ) a pop foldl
a max_stack : a popped ( max ) a pop foldl
a b concat_back ? b is_empty : a ? a b pop push b popped concat_back
b concat : b reverse concat_back
