"""

Childrens Sudoku Solver (CSE 307 Assignment 1)
Written by Arjun G. Menon (SBID: 106335011)
Written in Python 3.1 <--- Important !!!

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
 
"""


matrix = []

def solve():
    pass

#
# Readers:
#

def elem(i, j):
    if i>0 and j>0:
        return (i-1)*6 + (j-1)
    else:
        print("Error: invalid argument to function elem(i, j)")

def feed(x):
    if x=='-':
        matrix.append({1:True, 2:True, 3:True, 4:True, 5:True, 6:True})
    if x>='1' and x<='6':
        d = {1:False, 2:False, 3:False, 4:False, 5:False, 6:False}
        if x != '-':
            d[int(x)] = True
        matrix.append(d)

#
# Printers:
#

def str_elem_list(d):
    l = []
    for i in range(1, 6+1):
        if d[i]:
            l.append(i)
    return l

def str_elem_char(d):
    e = '-'
    for i in range(1, 6+1):
        if d[i]:
            if e == '-':
                e = str(i)
            else:
                e = '*'
                break
    return e
        
def display_matrix(str_elem):
    for i in range(1, 6+1):
        for j in range(1, 6+1):
            print(str_elem(matrix[elem(i, j)]), end=' ')
        print()

#
# File access sequence:
#

file = open("input.txt")

for line in file:
    for k in line:
        feed(k)

solve()

display_matrix(str_elem_list)
print()
display_matrix(str_elem_char)
