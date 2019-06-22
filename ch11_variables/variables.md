# Concepts
* variables will be immutable -- even when we "reassign" a variable, we just delete the old one and replace it
* variables name (or point to?) values
* environment: the dictionary (?) of names and their corresponding values for our program -- each instance of our program will have a new environment
# Symbols Syntax
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
  3. strings -- malloc and strcpy
  4. lists -- allocate and then copy item by item.