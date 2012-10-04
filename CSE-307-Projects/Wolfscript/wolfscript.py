
import sys
import tpg

class SemanticError(Exception):
    """
    This is the class of the exception that 
    is raised when a semantic error occurs.
    """
    
    def __init__(self, desc=None):
        self.err_desc = desc
    
    def __repr__(self):
        if self.err_desc == None:
            return "SEMANTIC ERROR"
        else:
            return "Error: " + self.err_desc


class ReturnValue(SemanticError):
    """
    This is used by a function to return a value to its caller.
    """
    
    def __init__(self, val):
        self.val = val
        self.err_desc = "Only a function can return a value."


class Node(object):
    """
    A base class for nodes. Might come in handy in the future.
    """

    def evaluate(self, context):
        """
        Called on r-value children of Node to evaluate that child.
        """

        raise Exception("Not implemented.")
    
    def setval(self, val):
        """
        Called on l-value children of Node to set value. 
        """

        raise Exception("Not implemented.")


class Value(Node):
    """
    A node representing integer, string and array values.
    """

    def __init__(self, value):
        if isinstance(value, list):
            self.value = value
        elif str(value)[0]=='"':
            self.value = value[1:len(value)-1]
        else:
            self.value = int(value)

    def evaluate(self, context):
        if isinstance(self.value, list):
            rl = []
            for k in self.value:
                rl.append(k.evaluate(context))
            return rl
        else:
            return self.value


class Assignment(Node):
    """
    A node representing assignment statements.
    """
    
    def __init__(self, left, right):
        self.left = left
        self.right = right
    
    def evaluate(self, context):
        self.left.setval(self.right.evaluate(context), context)


class Variable(Node):
    """
    A node representing a variable in the r-value.
    """
    
    def __init__(self, name):
        self.name = name
    
    def evaluate(self, context):
        # Get the value of the variable
        try:
            return context[self.name]
        except KeyError:
            raise SemanticError(self.name + " not found.")
    
    def setval(self, val, context):
        context[self.name] = val


class ArrayIndex(Node):
    """
    A node representing an array indexing operation.
    """
    
    def __init__(self, left, right):
        # The nodes representing the left and right sides of this MathOp.
        self.left = left
        self.right = right

    def evaluate(self, context):
        left = self.left.evaluate(context)
        right = self.right.evaluate(context)
        
        if isinstance(right, int):
            if isinstance(left, list) or isinstance(left, str):
                try:
                    return left[right]
                except IndexError:
                    pass
                
        raise SemanticError("Index out of bounds.")
    
    def setval(self, val, context):
        left = self.left.evaluate(context)
        right = self.right.evaluate(context)
        
        if isinstance(right, int):
            if isinstance(left, list):
                try:
                    left[right] = val
                    return
                except IndexError:
                    pass
                
        raise SemanticError("Index out of bounds.")


class MathOp(Node):
    """
    A node representing a mathematical operation.
    """
    
    def __init__(self, left, op, right):
        # The nodes representing the left and right sides of this MathOp.
        self.op = op
        self.left = left
        self.right = right
    
    def evaluate(self, context):
        left = self.left.evaluate(context)
        right = self.right.evaluate(context)

        if isinstance(left, int):
            if isinstance(right, int):
                if self.op == 'or': # Boolean OR.
                    return 1 if (left > 0 or right > 0) else 0
                elif self.op == 'and': # Boolean AND.
                    return 1 if (left > 0 and right > 0) else 0
                elif self.op == 'not': # Boolean NOT.
                    return 1 if left == 0 else 0
                elif self.op == '<':
                    return 1 if left < right else 0
                elif self.op == '==':
                    return 1 if left == right else 0
                elif self.op == '>':
                    return 1 if left > right else 0
                elif self.op == 'xor': # Bitwise XOR.
                    return left ^ right
                elif self.op == '+':
                    return left + right
                elif self.op == '-':
                    return left - right
                elif self.op == '*':
                    return left * right
                elif self.op == '/':
                    if right == 0:
                        raise SemanticError("Division by zero.")
                    return left / right
        
        if((isinstance(left, str) and isinstance(right, str))
        or (isinstance(left, int) and isinstance(right, str))
        or (isinstance(left, str) and isinstance(right, int))):
            left = str(left)
            right = str(right)
            if self.op == '+':
                return left + right
        
        raise SemanticError("Invalid MathOp.")


class Block(Node):
    """
    A node representing a block.
    """
    
    def __init__(self, stmts):
        self.stmts = stmts

    def evaluate(self, context):
        for i in self.stmts:
            i.evaluate(context)


class If(Node):
    def __init__(self, expr, stmt):
        self.expr = expr
        self.stmt = stmt
    
    def evaluate(self, context):
        if self.expr.evaluate(context):
            self.stmt.evaluate(context)


class While(Node):
    def __init__(self, expr, stmt):
        self.expr = expr
        self.stmt = stmt

    def evaluate(self, context):
        while self.expr.evaluate(context):
            self.stmt.evaluate(context)


class Print(Node):
    def __init__(self, expr):
        self.expr = expr
    
    def evaluate(self, context):
        print self.expr.evaluate(context)


class Return(Node):
    def __init__(self, expr):
        self.expr = expr
    
    def evaluate(self, context):
        raise ReturnValue(self.expr.evaluate(context))


class Function(Node):
    def __init__(self, args, body):
        self.args = args
        self.body = body


class FunctionDefinition(Node):
    def __init__(self, fvar, args, body):
        self.fvar = fvar
        self.func = Function(args, body)
    
    def evaluate(self, context):
        self.fvar.setval(self.func, context)


class FunctionCall(Node):
    def __init__(self, fvar, args):
        self.fvar = fvar
        self.args = args
    
    def evaluate(self, context):
        func = self.fvar.evaluate(context)
        
        if not isinstance(func, Function):
            raise SemanticError(self.fvar.name + " is not a function.")
        
        if len(self.args) != len(func.args):
            raise SemanticError("Function argument count mismatch.")
        
        context2 = dict(context)
        
        for i in range(0, len(self.args)):
            Assignment(func.args[i], self.args[i]).evaluate(context2)
        
        try:
            func.body.evaluate(context2)
        except ReturnValue as ret:
            return ret.val
        

class Comment(Node):
    def __init__(self, text):
        pass
    
    def evaluate(self, context):
        pass


# This is the TPG Parser that is responsible for turning our language into
# an abstract syntax tree.
class Parser(tpg.Parser):
    """
    token value "(\d+)|(\\"([^\\"])*\\")" Value;
    token variable '[A-Za-z][A-Za-z0-9]*' Variable;
    separator space "\s+";
    
    START/a -> statement/a ;    
    
    statement/a -> ( function_definition/a | block/a | if_stmt/a | while_stmt/a | line_stmt/a ) ;
    
    function_definition/a -> variable/v arg_names/n block/b    $ a = FunctionDefinition(v, n, b) $ ;
    
    block/a -> '{'   $ a = Block([])     $ 
       ( statement/b $ a.stmts.append(b) $ )* 
    '}' ;
    
    line_stmt/a -> ( assignment/a | print_stmt/a | return_stmt/a | function_call/a ) ';' ;
    
    if_stmt/a -> "if" "\(" expression/e "\)" statement/s       $ a = If(e, s) $;
    while_stmt/a -> "while" "\(" expression/e "\)" statement/s $ a = While(e, s) $;
    assignment/a -> expression/a "=(?!=)" expression/b         $ a = Assignment(a, b) $ ;
    print_stmt/a -> 'print ' expression/a                      $ a = Print(a) $ ;
    return_stmt/a -> 'return ' expression/a                    $ a = Return(a) $ ;
    function_call/a -> variable/v arg_list/l                   $ a = FunctionCall(v, l) $ ;
    
    expression/a ->  boolOR/a ;
    boolOR/a -> boolAND/a ( "or"/op boolAND/b                 $ a = MathOp(a, op, b) $ )* ;  
    boolAND/a -> boolNOT/a ( "and"/op boolNOT/b               $ a = MathOp(a, op, b) $ )* ;     
    boolNOT/a -> comparison/a | "not"/op expression/b         $ a = MathOp(b, op, b) $    ;
    comparison/a -> xor/a ( ("<"/op | "=="/op | ">"/op) xor/b $ a = MathOp(a, op, b) $ )* ;
    xor/a -> addsub/a ( "xor"/op addsub/b                     $ a = MathOp(a, op, b) $ )* ;
    addsub/a -> muldiv/a ( ("\+"/op | "\-"/op) muldiv/b       $ a = MathOp(a, op, b) $ )* ;
    muldiv/a -> index/a ( ("\*"/op | "/"/op) index/b          $ a = MathOp(a, op, b) $ )* ;
    index/a -> parens/a ( "\[" expression/b "\]"              $ a = ArrayIndex(a, b) $ )* ; 
    
    parens/a -> function_call/a | "\(" expression/a "\)" | literal/a ;
    
    literal/a -> value/a | variable/a | array/a ;
    
    array/a -> "\["    $ a = Value([]) $ 
          expression/b $ a.value.append(b) $ 
    ( "," expression/b $ a.value.append(b) $ )* 
    "\]" ;
    
    arg_names/a -> "\(" $ a = [] $
       (     variable/v $ a.append(v) $ )?
       ( "," variable/v $ a.append(v) $ )*
    "\)" ;
    
    arg_list/a -> "\(" $ a = [] $
    (     expression/e $ a.append(e) $ )?
    ( "," expression/e $ a.append(e) $ )*
    "\)" ;
    
    """

# Make an instance of the parser. This acts like a function.

parse = Parser()

# This is the driver code, that reads in lines, deals with errors, and
# prints the output if no error occurs.

# Open the file containing the input.
f = file(sys.argv[1], "r")
code = f.read()
f.close()

code = "{" + code + "}"

# For each line in f,
#for l in f:

try:
    # Try to parse the expression.
    node = parse(code)

    # Evaluate
    node.evaluate({})

# If an exception is thrown, print the appropriate error.
except tpg.Error as e:
    print "Syntax Error at line " + repr(e.line) + " column " + repr(e.column) + ": " + e.msg 

    # Uncomment the next line to re-raise the syntax error,
    # displaying where it occurs. Comment it for submission.

    # raise

except SemanticError as err:
    print repr(err)

    # Uncomment the next line to re-raise the semantic error,
    # displaying where it occurs. Comment it for submission.

    # raise

f.close()

