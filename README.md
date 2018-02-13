# SOFUNLANG
This repository contains my REPL for a stack-oriented functional language in reverse polish notation. See the language specification for more information about the language itself. 
Just to get an idea of the syntax: the following is the solution of the first task of projecteuler.net "Find the sum of all the multiples of 3 or 5 below 1000.".

    num sums ? 1000 num = : 0 ? 3 num % ~ 5 num % ~ | : num num 1 + sums + ? num 1 + sums
    
The above function gives you the correct result, when executed as "1 sums". 
As you see, due to reverse polish notation SOFun's code isn't really readable. Then again, all of it's syntax just consists of ':'s and '?'s: no braces, no semicolons, no keywords. Just two symbols and a few built-in functions that behave exactly the same as any other function you could write yourself. That doesn't make it an easier language to write programs in, but isn't it kind of cool nonetheless? ;)
