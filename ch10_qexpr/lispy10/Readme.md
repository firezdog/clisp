In this chapter we add Q- (or *quoted*) expressions, expressions enclosed in brackets that are stored but not immediately evaluated.  Q-expressions are supposed to correspond to the macro expressions available in other lisps.

A Q-expression is a series of expressions enclosed in braces:

>  qexpr  : '{' \<expr>* '}' ;

When constructing a Q-expression, we call the constructor for S-expressions, changing the type only to LVAL_QEXPR.

Printing and deletion are almost the same as for S-expressions.  Minor tweaks must be made to *lval_add* and *lval_del*.  Listing two cases after each other in a switch statement allows us to re-use the code for deleting S-expressions, but I have also added a separate function, *delete_cells*, to clean up the code in *lval_del*.