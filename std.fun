#hoch
exp base ^ ? exp 1 < : 1 ? base exp 1 - base ^ *

#kleiner gleich
a b <= ? a b < : 1 ? a b = : 1 ? 0

#größer gleich
a b >= ? a b > : 1 ? a b = : 1 ? 0

#factorial
a ! ? a 1 = : 1 ? a a 1 - ! *

#misc
a dup : a a
a b first : a
a b second : b
a b min ? a b < : a ? b
a b max ? a b > : a ? b

