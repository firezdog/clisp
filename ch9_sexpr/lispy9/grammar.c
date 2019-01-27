#include "lispy.h"

void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* Operator, mpc_parser_t* Sexpr, mpc_parser_t* Expr, mpc_parser_t* Lispy) {
    mpca_lang(MPCA_LANG_DEFAULT,
    "numeral: /-?[0-9]+/ ;\
    operator: '+' | '-' | '*' | '/' ;\
    sexpr: '(' <expr>* ')' ;\
    expr: <numeral> | <operator> | <sexpr> ;\
    lispy: /^/ <expr>* /$/ ;",
    Numeral, Operator, Sexpr, Expr, Lispy);
}