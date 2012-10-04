"""

Childrens Sudoku Solver (CSE 307 Assignment 1)
Written by Arjun G. Menon (SBID: 106335011)
Language: Python 2.6.1

Short Description
~~~~~~~~~~~~~~~~~

There are 6 boxes. In each box (i,j) must satisfy the following conditions:

 /-----------------------------------\
 | (i>=1 and i<=3) | (i>=4 and i<=6) |
 |       and       |       and       |
 | (j>=1 and j<=2) | (j>=1 and j<=2) |
 |-----------------------------------|
 | (i>=1 and i<=3) | (i>=4 and i<=6) |
 |       and       |       and       |
 | (j>=3 and j<=4) | (j>=3 and j<=4) |
 |-----------------------------------|
 | (i>=1 and i<=3) | (i>=4 and i<=6) |
 |       and       |       and       |
 | (j>=5 and j<=6) | (j>=5 and j<=6) |
 \-----------------------------------/

    The variable 'matrix' is a list that represents the 6x6 sudoku.
    The index-element correspondence of this matrix is shown below:
 
     0  1  2  3  4  5 
     6  7  8  9 10 11 
    12 13 14 15 16 17 
    18 19 20 21 22 23 
    24 25 26 27 28 29 
    30 31 32 33 34 35
 
"""

matrix = []
zones = []

def construct_zones():
    """There are 18 zones in total."""
    # First 6 zones - the rows
    for i in range(1, 6+1):
        zones.append(map((lambda j: elem(i, j)), range(1, 6+1)))
    # Second 6 zones - the columns
    for j in range(1, 6+1):
        zones.append(map((lambda i: elem(i, j)), range(1, 6+1)))
    # Third 6 zones - the boxes
    for i in range(3):
        for j in range(2):
            first_box = [(1,1), (1,2), (1,3),
                         (2,1), (2,2), (2,3)]
            zones.append(map((lambda e: apply(elem, e)), 
                         map((lambda x: (2*i + x[0], 3*j + x[1])), first_box)))

def reduce(el):
    did_something = False
    for ex in el:
        if len(matrix[ex]) == 1:
            for ey in el:
                if len(matrix[ey]) > 1:
                    try:
                        matrix[ey].remove(matrix[ex][0])
                        did_something = True
                    except ValueError:
                        pass
    return did_something
                    
def reduce_all():
    actions = 0
    for each in zones:
        if reduce(each):
            actions += 1
    if actions > 0:
        return True
    else:
        return False

def solve():
    while True:        
        if not reduce_all():
            break

#
# Readers:
#

def elem(i, j):
    if i > 0 and j > 0:
        return (i-1)*6 + (j-1)
    else:
        print "Error: invalid argument to function elem(i, j)"
        raise ValueError

def feed(x):
    if x == '-':
        matrix.append([1, 2, 3, 4, 5, 6])
    elif x >= '1' and x <= '6':
        matrix.append([int(x)])

#
# Printers:
#
       
def display_matrix():
    for i in range(1, 6+1):
        for j in range(1, 6+1):
            print matrix[elem(i, j)],
        print ""

def display_result():
    for i in range(1, 6+1):
        for j in range(1, 6+1):
            symbol = '-'
            length = len(matrix[elem(i, j)])
            if length == 1:
                symbol = str(matrix[elem(i, j)][0])
            elif length > 1:
                symbol = '*'
            print symbol,
        print ""

#
# File access sequence:
#

file = open("input.txt")

for line in file:
    for k in line:
        feed(k)

construct_zones()
for each in zones:
    print each
solve()

print ""
display_matrix()
print ""
display_result()
