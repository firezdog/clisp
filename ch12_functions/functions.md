# What?
* a layer of abstraction used to capture two ideas: (1) command; (2) computation (black box)
* also -- partial computation (lambdas waiting to be filled) that may be composed (lambda calculus)
* idea: \ {\<variables>} {\<f(...variables...)>} e.g. \ {x y} {+ x y}, so that (\ {x y} {+ x y}) 10 20 => 30
* interacts with define: define {square} (\ {x} {* x x})
# Function Type
* lval function? 3 parts: (1) arguments (to be bound) (2) body (q-expr) (3) environment (where the arguments are interpreted)
* builtin functions have non-null lbuiltin pointer (and so are different from user defined)
# Lambda Function
# Parent Environment
# Function Calling
# Variable Arguments
# Interesting Functions
## Function Definition
## Currying