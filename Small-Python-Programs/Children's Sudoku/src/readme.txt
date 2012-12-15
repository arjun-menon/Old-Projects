Hello,

My name is Arjun G. Menon.
My ID number is 106335011.

My home page is at arjungmenon.com

I skimmed through the first section of this wikipedia 
article (http://en.wikipedia.org/wiki/Algorithmics_of_sudoku).

Short Description
~~~~~~~~~~~~~~~~~

In my program I use a list called 'matrix' to represent the puzzle. 
Each element in this list corresponds to an element in the sudoku matrix
The index-element correspondence of 'matrix' to a 6x6 sudoku is shown below:
 
     0  1  2  3  4  5 
     6  7  8  9 10 11 
    12 13 14 15 16 17 
    18 19 20 21 22 23 
    24 25 26 27 28 29 
    30 31 32 33 34 35

The value of each element of 'matrix'  is a list of allowed values 
for that particular element.

At first my program constructs a list called 'zones' which contain the 
lists of indices of regions that must contain a unique set of numbers.
In childern's sudoku there are 18 zones. (6 rows, 6 colums, 6 boxes)

Then the program solves the puzzle by removing from each element's list, 
values that are not allowed by looking at the originally given values.
    