#fibonacci
n num1 num2 fib_back ? n 0 = : num2 ? n 1 - num2 num1 num2 + fib_back
n fib : n 1 - 0 1 fib_back

#first task from projecteuler.net: "Find the sum of all the multiples of 3 or 5 below 1000."
num sums_back ? 1000 num = : 0 ? num 3 % ~ num 5 % ~ | : num num 1 + sums_back + ? num 1 + sums_back
sums : 1 sums_back

#disan count
n disan ? n 2 = : 2 ? n 2 % ~ : n 1 - disan n ? n 1 - disan
