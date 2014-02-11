WolfScript
==========
Wolfscript is a JavaScript-like language I developed as an undergraduate at [Stony Brook University](http://www.stonybrook.edu/), in [a class](http://www.cs.stonybrook.edu/~liu/cse307/) on the principles of programming languages.

Wolfscript typifies the modern scripting language. It supports a variety of high-level language features such as array literals, [recursive](https://en.wikipedia.org/wiki/Recursion_(computer_science)) function calls, [nested function](https://en.wikipedia.org/wiki/Nested_function) definitions, [higher order functions](https://en.wikipedia.org/wiki/Higher-order_function), etc.

In Wolfscript, strings and integers are passed by value, while arrays are passed by reference/sharing. Variables are scoped [dynamically](https://en.wikipedia.org/wiki/Lexical_scoping#Dynamic_scoping), i.e. the outer scope is the scope of the _caller_.

Here is some sample _WolfScript_ code:

	print "Firing up..";
	print "";

	main()
	{
		data = [ [ 100, 42 ], [ 100, 50 ], [ 123, 456 ], [ 300, 9000 ] ];

		a3input2()
		{
		    result = [ 0, 0, 0, 0 ];

		    i = 0;
		    while (i < 4) {
		        a = data[i][0];
		        b = data[i][1];

		        if (a > 0) {
		            while (b > 0) {
		                if (a > b) {
		                    a = a - b;
		                }
		                if (not (a > b)) {
		                    b = b - a;
		                }
		            }
		        }

		        result[i] = a;
		        i = i + 1;
		    }

		    return result;
		}

		print "Output of Assignment 3 input set 2: ";
		print a3input2();
		print "";

		mul(x, y)
		{
			return x*y;
		}

		print "Example of a function with arguments";
		print mul(42,11);
		print "";

		factorial(n)
		{
			if (n < 2 ) {
				return 1;
			}
			if (not (n < 2)) {
				return n*factorial(n-1);
			}
		}

		print "Example of recursive function: ";
		print "Factorial of " + mul(2,3) + " is " + factorial(mul(2,3));
		print "";

		prettyprinter(n, f) { print "Factorial of " + n + " is " + f; }

		a3input1higherorder(foo, moo)
		{ 
		    i = 1;
		    while (i < 10) {
		        if (not i == 5) {
		             moo(i, foo(i));
		        }
		        i = i + 1;
		    }
		}

		print "Example of higher order functions:";
		a3input1higherorder(factorial, prettyprinter);
		print "In the above a3input1higherorder (a modified version of assignmenr 3 input set 1 ";
		print "is a higher order function that takes two other functions as arguments.";
		print "";


		ff(uu) { uu[1] = 9; yy[1] = 77; }
		gg() {
			xx = [1,"g",3]; yy = [11, "hh", 33];
			print xx;
			print yy;

			ff(xx);
			print xx;
			print yy;
		}
		print "Example showing pass-by-sharing for arrays and dynamic scoping: ";
		gg();
	}

	main();


Note
----
This is one of the best classes I've taken at [Stony Brook](http://www.cs.sunysb.edu/). It was taught by [Tom Rothamel](http://rothamel.us), who was an adjunct professor at the time. Tom and I have maintained a friendship outside of class, since then.

Through Tom, I met [Y. Annie Liu](http://www.cs.stonybrook.edu/~liu/), my future undergraduate research adviser.
