# SOFUNLANG
This repository contains my REPL for a stack-oriented functional language in reverse polish notation. The combination of these two means: minimal syntax without sacrifice - okay: human readability, maybe ;)
Just to get an idea of the syntax: the following is the solution of the first task of projecteuler.net, "Find the sum of all the multiples of 3 or 5 below 1000.".

    num sums ? 1000 num = : 0 ? num 3 % ~ num 5 % ~ | : num num 1 + sums + ? num 1 + sums
    
The above function gives you the correct result, when executed as "1 sums". 
As you see, due to RPN (reverse polish notation) SOFUNLANG's code needs no structuring syntax (apart from the function declaration). That's because RPN is a format, that can be fully parsed as a linear stack, without any nested brackets. All of it's syntax just consists of ':'s, '?'s, and opening and closing brackets if you need data stacks: no semicolons, no keywords. Just four symbols and a few built-in functions that behave exactly the same as any other function you could write yourself.
See the language specification for further information about the language. 

You can compile the source with "g++ -o sfrepl sofunrepl.cpp -lreadline -lstdc++ -std=c++11".

