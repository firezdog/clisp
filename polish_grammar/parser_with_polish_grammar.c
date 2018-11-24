#include "../mpc_library/mpc.h"

mpc_parser_t* Numeral = mpc_new("numeral");
mpc_parser_t* Operator = mpc_new("operator");
mpc_parser_t* Expr = mpc_new("expr");
mpc_parser_t* Lispy = mpc_new("lispy");

mpca_lang(MPCA_LANG_DEFAULT,
"numeral: /-?[0-9]+/    ; \
operator: '+' | '-' | '*' | '/' ; \
expr: <numeral> | '(' <operator> <expr>+ ')' \
lispy: /^/ <operator> <expr>+ /$/",
Numeral, Operator, Expr, Lispy);

/* program = start of input (^) & operator & expression+ & end of input ($)
expression = number or: ( & operator & expression+ & )
operator = + or - or * or /
number = -* and 0-9+

regex

number = -?[0-9]+
operator [+?-?*?/*]{1}

use '|' and strings when defining an operator instead of a regex
more precise (and correct) to call them 'numerals' rather than 'numbers'
'()' not needed when defining scope of '|' ?
one or more expression = <expr>+ is needed.
This doesn't define a semantics because an operator + 3 expressions will be well formed even if it's binary */