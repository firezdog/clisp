# Overview

In this section we built out the *lval* data type to accomodate S(ymoblic)-Expressions -- basically any numeral, symbol (I retained the name "operator"), or combination of numerals and symbols.  Accordingly, numerals are no longer handled as "raw" longs / doubles, but incorporated into the *lval* data type as s-expressions.  An *lval* can now be an error, a numeral (as a kind of atom), or an s-expression, which stores as children other s-expressions in "cells".

As we expanded the *lval* datatype we also introduced proper methods for allocating and freeing memory.  When an *lval* is initialized, memory is allocated to hold it.  When adding to the *cell* of an *lval*, memory is reallocated for the cell according to the number of items held within it.  When an *lval* has been processed and is no longer needed, it is deleted: if it contains an error or a sym, the (string / char*) contents are freed; if it holds children in its cell, each of the children are deleted; then the *lval* itself is deleted.

The two major data types the program uses, at this point, then, are *lval*'s and *mpc_ast_t* or grammatical trees generated from user input.  A user types in some string, which the *mpc* library parses into tree form (*mpc_ast_t*).  We then hand the tree off to *lval_read*, which converts the tree into an *lval*.

*lval_read* accomplishes the conversion as follows.  It does have pathways to handle different states of the node it encounters -- operators (symbols), numerals, and s-expressions -- but this doesn't give us a great sense of what it actually does to the typical tree that it receives or, for that matter, what said tree looks like.

If you look at *grammar.c*, you can get a sense of how the grammar is (recursively) structured.  At the top level is always a "lispy", which is composed of one or more expressions.  I am guessing that the children of a lispy, then, are the expressions it contains.  If an expression is a numeral or operator, it doesn't have children.  If it's an s-expression, then it does: numerals, operators, and other s-expressions.

So if we think about a typical "lispy" as it might be used by the program now, we have some arithmetical statement expressed in Polish notation:

\> \+ 3 \(\+ (* 5 7) (- 4 3))

">" denotes the top level and has as its child the lispy whose children are "+", "3", and the expression in parentheses whose children are etc.

The first tag, then is ">", so *lval_read* creates a new *lval* of type "s-expression".  It then iterates through the children and adds them to the s-expression's cell (children).  In the case above, the first cell holds "+", the second "3", and the third another s-expression.  (It's interesting to note here, that each unit defined in a grammatical entry -- and not only the entities that satisfy entries -- would seem to count as a child -- so an s-expression has as children two parentheses and each of the expressions that come between them.  This is why we include logic to skip parentheses when iterating through a grammatical node's chidlren.)

In short, *lval_read* creates a new structure, an *lval*, which is isomorphic to the old structure, the *mpc_ast_t* -- but processes its contents in such a way that they are now available for calculation.  The evaluation function, which was also added here, demonstrates how we can give a semantics for the expressions as they have so far been defined: simple arithmetic.  Other operations (such as finding the minimum or maximum of a series of numbers) are also possible.