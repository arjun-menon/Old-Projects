
import string, tpg
from functools import reduce

class Node(object):
    def apply(self, args):
        raise Exception("Not implemented.")

class Symbol(Node):
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return '<' + self.name + '>'

class Literal(Node):
    def __init__(self, val):
        self.val = val
    def __repr__(self):
        return '"' + self.val + '"' if isinstance(self.val, str) else str(self.val)
    def apply(self, args):
        return List([self] + args.lst)

class List(Node):
    def __init__(self, lst):
        self.lst = lst
    def __repr__(self): 
        return reduce(lambda a, b: a+' '+b, [repr(k) for k in self.lst], "(") + " )"
    def run(self):
        return hd(self).apply(tl(self))

def hd(l):
    return l.lst[0]
def tl(l):
    return List(l.lst[1:]) if len(l.lst) > 2 else l.lst[1]

builtin_funcs = { 'hd' : hd, 'tl' : tl }

builtin_funcs['pyeval'] = lambda l: List([ Literal(i) for i in 
                                      [eval(s.val) for s in [e.run() for e in l] 
                                       if isinstance(s, Literal) and isinstance(s.val, str)] ])

class Parser(tpg.Parser):
    """
    token number '(\d+)' int
    token string "(\\"([^\\"])*\\")" str;
    token symbol '[A-Za-z][A-Za-z0-9]*' str;
    separator space "\s+";
    
    START/a -> expr/a ;
    expr/a -> term/a | "\(" $ a = List([]) $ ( expr/b $ a.lst.append(b) $ )* "\)" ;
    term/a -> number/t $ a = Literal(t) $ 
            | string/t $ a = Literal(t[1:len(t)-1]) $ 
            | symbol/t $ a = Symbol(t.lower()) $ ;
    """

parse = Parser()

#f = file(sys.argv[1], "r")
#code = f.read()
#f.close()
#code = "(1 (3 4) 2)"
code = """
(2 a 3)
"""
#((pyeval "2") 4)

try:
    prog = parse(code)
except tpg.Error as e:
    print("Syntax Error at line " + repr(e.line) + " column " + repr(e.column) + ": " + e.msg)
    raise 

print('\n', prog)
print('\n', prog.run())