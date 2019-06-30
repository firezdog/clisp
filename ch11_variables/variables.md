# Immutability
* variables will be immutable -- even when we "reassign" a variable, we just delete the old one and replace it
* variables name (or point to?) values
* environment: the dictionary (?) of names and their corresponding values for our program -- each instance of our program will have a new environment
# Symbol Syntax
* allow more options for symbols:

```
/[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/
```

Note that above that we need to double escape back-slashes (because of C), and because "-" expresses range, we need to escape that too.  So we allow letters, numerals, underscores, arithmetic symbols, backslashes, operators "=", "<", ">", "!", and "&".

Note also that this replaces our previous method for handling symbols -- so we will need to make the different operators part of the environment as mentioned above (?)
# Function Pointers
* new lval using c function pointers (point to a function)

```
typedef lval*(*lbuiltin)(lenv*, lval*)
```

This means (roughly) that dereferencing the lbuiltin function pointer and calling it with an lenv* and lval* gives an lval* (or a pointer to an lval).

Or: get the function from the pointer, call it with the right values, and you get an lval* back.
# Cyclic Types
We have to use forward declaration (for me I put everything in a header file) to avoid cycles in our type declarations -- otherewise we run into situations where the definition of A depends on the definition of B and the definition of B depends on the definition of A (A has already to be defined for the compiler to understand our definition of B, and vice versa).

See lispy.h
# Function Type
* Constructors will now have to initialize the lval_fn field.
* we make a constructor for lval_fn -- store the function in the new field.
* we add a case for lval deletion (that doesn't do anything special).
* we add a case for the printer (or our show method) -- say it's a function.
* we need a function to copy things -- useful for putting them into and taking them out of the environment.  We can copy the following
  1. numbers -- field
  2. functions -- field
  3. strings (i.e. errors) -- malloc and strcpy
  4. lists -- allocate and then copy item by item.
# Environment
* A kind of dictionary relating names and values (see lis.py)
* strategy -- two arrays of equal length -- for a variable s, find n in keys such that keys[n] = s, then values[n] will be the value for s.  (Offhand this suggests to me that variable lookup is going to be linear time?)
* we define lenv accordingly and create a constructorn / destroyer
* then we're going to need to functions to add to variables and assignment fields and associate them?
* ^ yes -- one function to add to the environment (lenv_put), another to read from it (lenv_get)
* My question: why is it necessary to have a separate list of variables at all, given that lval's now have a variable field built in?  Maybe I can experiment in a later version with an abridged lenv
# Variable Evaluation
* Update the evaluation function by (1) passing in an environment and (2) looking up symbols within that environment.  (? -- where does the environment get instantiated -- I guess where the function is first called?)
* I'm guessing this replaces builtin() -- now nothing will be built in -- we will just have predefined variables
* Maybe -- but really we're updating lval_eval to explicitly check symbols and evaluate them using the environment.
* Yes -- the lval_eval_sexpr() function now takes an environment.
* But we use the callback to evaluate (this is where lbuiltin comes in)
# Builtins
* this leads to the need to "register" our builtins (by creating an environment with predefined entries) instead of putting them in a function
* redo builtin functions.  How?

```
lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}
```

We now have a builtin_add function that calls builtin_op with an environment, an lval, and "+" -- it will then be processed normally.  (What does builtin_op do with the environment?)
* builtin still gets called, we just have a variety of builtin_x as intermediaries (will be associated with default vals in env)
* registration: for each builtin, call lval_put with needed var
* this involves creating lval's for the symbols and functions, then deleting them (sub-function)
* seems like basically we need to associate one of our builtin funcs with a pointer and then put that in an lval in the env
* this is where the magic of our builtin declaration comes in -- it allows us to pass functions with pointers.  See above.
* we register everything in lispy.c when starting the app

* Remember that on input we parse the input and then send it to lval_eval, which will know look up operations from the environment, on the one hand, evaluate full expressions, on the other.  So now lval_eval gets an environment and looks up symbols in the environment
* (It's strange to me that lval_eval calls a function that then calls it -- kind of indirect recursion)
* lval_eval will first receive an sexpr (most likely?) from lval_read
* we also change lval_eval_sexpr (the "helper" function) to evaluate with functions instead of directly "routing" symbols
* note that now, in the process of evaluating s-expressions, all their symbols get replaced with variable assignments from the environment
# Define Function
* We will allow a user to define her own variables
* Syntax: define \<quoted name> \<value>
* associate a list of symbols with a series of values
* check that input types are correct
* iterate through each symbol and put corresponding value in environment (what if they are unequal?), return () on success or else error
# Error Reporting
* We want to create something like printf that will allow us to pass multiple values to the error reporter -- then it can give us specific information about what went wrong
* I did a little of this with the macros, but admittedly not very robustly.
* add ... -- rest operator (not related to REST -- it gives us the *rest* of the arguments, cf. JavaScript) to the lval_err constructor
* va_start, va_arg, and va_end are used as part of "..." api
* allocate a buffer of 512 bytes for the finished string (we will shrink if less -- but we will not allow more.)
* we use vsnprintf to put the arguments into our format string and copy that into the field. (this all sounds complicated, but we're just leveraging a builtin c api -- or macros)
* we will also use our new lva_err in macros -- but using __VA_ARGS__ to get all the args instead (we can use rest operator in the macro)
* for reporting of type errors, we should add a helper function that tells us what type of argument was passed into a function.