#fibonacci
n num1 num2 fib_back ? n 0 = : ( num2 ) ? n 1 - num2 num1 num2 + fib_back num2 push
n fib : n 1 - 0 1 fib_back

#disan count
n disan ? n 2 = : ( 2 ) ? n is_even : n 1 - disan n push ? n 1 - disan

#first task from projecteuler.net: "Find the sum of all the multiples of 3 or 5 below 1000."
num sums35_back ? 1000 num = : 0 ? 3 num is_div 5 num is_div | : num num 1 + sums35_back + ? num 1 + sums35_back
sums35 : 1 sums35_back

#second task from projecteuler.net: "By considering the terms in the Fibonacci sequence whose values do not exceed four million, find the sum of the even-valued terms.":
count n even_fibs_back ? count fib pop n > : 0 ? count fib pop is_even : count fib pop count 1 + n even_fibs_back + ? count 1 + n even_fibs_back
n even_fibs : 1 n even_fibs_back

#prime test (brute force)
a n is_prime_back ? a 2 = : 1 ? n 2 <= : a is_even ~ ? n a is_div ~ a n 1 - is_prime_back &
a is_prime : a a 1 - is_prime_back

#palindrome test
a is_palindrome ? a reverse a eq_stack : 1 ? 0

#quicksort
a less : a popped ( a pop <= ) filter
a greater : a popped ( a pop > ) filter
a quicksort ? a size 1 <= : a ? a less quicksort a pop push a greater quicksort concat
main : range
