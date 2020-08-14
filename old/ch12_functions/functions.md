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
* we are going to have to add error checking or the program will crash when the user calls with more arguments than are required -- but it's tricky, because this is only a problem if & is not included in the function def
* CASE: user doesn't supply extra arguments -- set extra args to empty list -- note -- this happens after we've run through all the a->cell's
# Interesting Functions
## Function Definition
* Problem: it's hard to define functions -- solution: write a lambda expression that will define new lambda expressions
  * user supplies name and formal arguments in one list (as well as definition?)
  * fact: the head of the list will be the name of the function, the tail with be the args.
  * def {func} (\ {signature body} {def (head signature) (\ (tail signature) body)})
    * e.g. func {add x y} {+ x y}
    * Why does this work?
      * def {func} (\ {x_0, x_1, ..., x_n} {/<x_0, x_1, ..., x_n...>})
      * we have a lambda followed by two lists -- a list of args and a list of an expression containing those args -- that is the requirement
      * so we have to show that the above call to lambda provides a list of args and a list of an expression containing those args -- and that ultimately it EVALUATES to the def. of a function.
      * the list of args is the function signature and body.
      * the expression containing them is a definition -- (inception style)
        * the definition gets the head of the signature, which is the function name, as its first argument
        * the definition then gets a lambda -- 
          * its list of args is the rest of the signature.
          * its expression is the body from the original definition (which is a list w/ expression)
## (Un)Currying
* Problem: "+" can take multiple args -- but what if we passed it args as a list?
  * Solution: add "+" to the list and use eval e.g.
    * eval (join {+} {1 2 3 4 5})
  * or generally we could have a function apply:
    * def {apply} (\ {func args} {eval (join func args)})
      * e.g. apply {+} {1 2 3 4 5} => 15
      * a slight improvement:
        * def {apply} (\ {func args} {eval (join (list func) args)})
          * e.g. apply + {1 2 3 4 5} => 15
* Above -- we had a function that doesn't take a list but we wanted to call it with a list.
  * What if we have a function that does take a list but we want to call it without a list?
  * def {supply} (\ {func & args} {func args})
    * e.g. 
      * supply head 1 2 3 4 5 => {1}
      * supply tail 1 2 3 4 5 => {2 3 4 5}
* This is called *currying* and *uncurrying* -- after Haskell Curry
## Others
* Get the first element in a list:
  * def {first} (\ {list} {eval (head list)})
* Get the second element in a list:
  * def {second} (\ {list} {first (head (tail list))})
* get the sum of the first two elements in a list :)
  * def {add-one-two} (\ {list} { + (first(list)) (second(list)) })