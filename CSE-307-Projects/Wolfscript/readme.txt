
CSE 307 Bonus Assignment

Arjun G. Menon

Language Features
-----------------

* The language supports recursive function calls.
* The language supports nested function definitions.
* Functions allow wolfscript statements given in assignment 3.

* String and integers are passed by value, while arrays are passed by sharing.
* Variables are scoped dynamically. (The outer scope is the scope of the caller.)

Example of function definition:

mul(x, y)
{
	print x*y;
}

Example of function call:

mul(3, 5);

You can find more example that cover nested functions, 
recursion, higher order functions, etc. in input.txt

Test Cases
----------
input.txt is one big program containing several test cases that verify that 
the interpreter implements all the above mentioned language features.
