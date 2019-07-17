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
* Functions have their own environment (for their bound variable), but they will also have a parent environment they can use to look up any variables that don't appear in their environment.  Parent environments chain (the global environment's parent is NULL)
* lenv_get will now look in parent environments if it doesn't find what it's looking for in the current environment.
* now, when we copy an lval, we need to copy its environment as well.
* defining a variable: we might now be defining a variable in the global environment or in a local environment.  In effect, we'll create a helper function that will walk up to the topmost environment for global definitions.
* we need to manage the builtins to allow users to distinguish between these two cases
# Function Calling
* lval_call will be our builtin function caller. It will perform the following steps:
  1. Bind variables formals
  2. evaulate the body using the environment
* we still have an issue: what if the function is supplied with too few / too many arguments on call?  Naive implementation causes crash
* we could return a partially bound function (i.e. another function)
# Variable Arguments
* use a special symbol (&) for extra variables at the end of a function call
* {x & xs} will take a single argument x and join additional arguments into a list called xs -- i.e. the additional arguments will be assigned to a list and stored in "xs"
  1. we need to convert the arguments after the first into a q-expression
  2. & must be followed by a symbol, if not throw an error
* I had a loop to slowly fill up the new list of arguments; orangeduck just uses builtin_list, converting the rest of "a" into a q_expr and putting it into the env directly.  Then he breaks because there's nothing left to do with "a".  Tried to test my solution, not sure it really works.
* It seems weird -- a function can be defined with an improper format and the interpreter does not stop us.
* my program was crashing because I was not deleting the param (?) / also I created a separate variable for builtin_list(e, a).  Not sure why.
  * doesn't seem like variable for builtin_list was a problem -- but deleting the variable was.  I thought maybe it would be because then environment isn't pointing to anything -- but no, because env points to a copy.  Maybe it's because builtin_list just makes "a" into a q_expr and returns it.  So if I delete it, there's nothing to delete later. That seems right, because if I comment out that line, there's no crash (though i guess there might be a memory leak?)
  * above shows why it MIGHT not be a good idea to store builtin_list
# Interesting Functions
## Function Definition
## Currying