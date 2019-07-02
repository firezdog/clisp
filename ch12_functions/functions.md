# What?
* a layer of abstraction used to capture two ideas: (1) command; (2) computation (black box)
* also -- partial computation (lambdas waiting to be filled) that may be composed (lambda calculus)
* idea: \ {\<variables>} {\<f(...variables...)>} e.g. \ {x y} {+ x y}, so that (\ {x y} {+ x y}) 10 20 => 30
* interacts with define: define {square} (\ {x} {* x x})
# Function Type
* lval function? 3 parts: (1) arguments (to be bound) (2) body (q-expr) (3) environment (where the arguments are interpreted)
* builtin functions have non-null lbuiltin pointer (and so are different from user defined)
* constructor -- make new environment for function, assign formals and body, builtin is NULL
* destroyer -- delete environment, formals, body, and free the function -- we are adding to this to our case, which formally just had "break"
* we're already using lval_fn to create our builtin functions -- we will use lval_lambda as our constructor and lval_del_lambda as our deconstructor.
* we also need to update lval_copy and printing function
* NOTE: we need to add a function lenv_copy -- we have to use these copy functions because otherwise we would get null pointer errors (we delete the original)
* printing: we print the formals (variables) and body -- we can also print the name, as with builtins (but I'm not sure which env these will be assigned to, so we can hold off) -- the fact that we can't check now whether one builtin pointer is equal to another is a problem -- we don't have any function that decides whether lval's are equal -- yet!
* that we now include "env" as argument to print function may become a problem later.
# Lambda Function
* (list -- formals, list -- body) => lval -- fn)
* this is allowing user to define lambdas -- we are not yet writing the code to evaluate expressions that contain user defined lambdas
* we need to check that the first list contains only symbols.  I wonder why there isn't a requirement that the second list contain the same number of symbols as the first?
# Parent Environment
# Function Calling
# Variable Arguments
# Interesting Functions
## Function Definition
## Currying