# Four Steps For Adding Features To Language
1. Add syntactical rule (recursive specification) -- in this case that a quoted expression is a q-expression.
2. Add representation (lval_qexpr)
3. Parsing functions (in lval_read -- initializes a qexpr based on mpc parsing and then reads it in just as if it were an sexpr)
4. Semantics -- lval_print updated, builtin functions like head, tail, etc.

# Writing Macros
We wrote the following macro -- (lval, boolean, string) => code

```
#define LASSERT(args, cond, err) \
    if(!(cond)) { lval_del(args); return lval_err(err); }
```

We can create specialized macros
1. for the case in which a builtin is called with incorrect number of args.
2. for the case in which a builtin is called with the empty list
* (builtins always take quoted expressions?)

String concatenation is possible with macros (and maybe without?)! Just put the strings together with a # before the variable you want to stringify, e.g.

```
#define BUILTIN_ARG_CHECK(item, num_args, function) \
    if(!(item->cell_count == num_args)) { lval_del(item); return lval_err(function " only accepts " #num_args " argument(s)"); }
```

(Above function is already passed as a string, but num_args was passed as an integer)

# New Builtins
* len -- returns the number of elements in a q-expression
* init -- for a list of length n, returns the first n-1 elements
* cons -- n+1, {1, 2, ..., n} => {n+1, 1, 2, ..., n} (where 1 through n+1 are values) -- this is like join and in fact we use lval_join to implement, BUT unlike join, it ADDS a value (non q-expr) to a q-expression.