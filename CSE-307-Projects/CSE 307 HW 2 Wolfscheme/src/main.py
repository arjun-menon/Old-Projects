import sys
import tpg

class SemanticError(Exception):
    """
    This is the class of the exception that is raised when a semantic error
    occurs.
    """
    
# These are the nodes of our abstract syntax tree.

class Node(object):
    """
    A base class for nodes. Might come in handy in the future.
    """

    def evaluate(self):
        """
        Called on children of Node to evaluate that child.
        """

        raise Exception("Not implemented.")

    
class Value(Node):
    """
    A node representing integer & string values.
    type = 0  means value is an integer
    type = 1  means value is an string
    """

    def __init__(self, value):
        if str(value)[0]=='"':
            self.value = str(value)
            self.type = 1
        else:
            self.value = int(value)
            self.type = 0

    def evaluate(self):
        return self.value

    
class Multiply(Node):
    """
    A node representing multiplication.
    """

    def __init__(self, left, right):

        # The nodes representing the left and right sides of this
        # operation.
        self.left = left
        self.right = right


    def evaluate(self):
        left = self.left.evaluate()
        right = self.right.evaluate()

        if not isinstance(left, int):
            raise SemanticError()
        if not isinstance(right, int):
            raise SemanticError()

        return left * right


class Divide(Node):
    """
    A node representing division.
    """

    def __init__(self, left, right):

        # The nodes representing the left and right sides of this
        # operation.
        self.left = left
        self.right = right

    def evaluate(self):
        left = self.left.evaluate()
        right = self.right.evaluate()

        if not isinstance(left, int):
            raise SemanticError()
        if not isinstance(right, int):
            raise SemanticError()

        if right == 0:
            raise SemanticError()
        
        return left / right


# This is the TPG Parser that is responsible for turning our language into
# an abstract syntax tree.
class Parser(tpg.Parser):
    """
    token value "(\d+)|(\\"\w+\\")" Value
    separator space "\s+";
    
#    START/a -> expression/a ;
    START/a -> literal/a ;

#    expression/a -> muldiv/a ;  
#    muldiv/a -> parens/a ( "\*" parens/b $ a = Multiply(a, b) $ | "/"  parens/b $ a = Divide(a, b) $ )* ;
#    parens/a -> "\(" expression/a "\)" | literal/a ;

    literal/a -> value/a;
    """

# Make an instance of the parser. This acts like a function.

parse = Parser()

# This is the driver code, that reads in lines, deals with errors, and
# prints the output if no error occurs.

# Open the file containing the input.
f = file(sys.argv[1], "r")

# For each line in f,
for l in f:

    try:
        # Try to parse the expression.
        node = parse(l)

        # Try to get a result.
        result = node.evaluate()

        # Print the representation of the result.
        print repr(result)

    # If an exception is thrown, print the appropriate error.
    except tpg.Error:
        print "SYNTAX ERROR"

        # Uncomment the next line to re-raise the syntax error,
        # displaying where it occurs. Comment it for submission.

        # raise
        
    except SemanticError:
        print "SEMANTIC ERROR"

        # Uncomment the next line to re-raise the semantic error,
        # displaying where it occurs. Comment it for submission.

        # raise

f.close()
        
