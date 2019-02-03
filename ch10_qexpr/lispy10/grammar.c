#include "lispy.h"

void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* Operator, mpc_parser_t* Sexpr, mpc_parser_t* Qexpr, mpc_parser_t* Expr, mpc_parser_t* Lispy) {
    mpca_lang(MPCA_LANG_DEFAULT,
    "numeral: /-?([0-9]*\\.)?[0-9]+/ ;\
    operator: '+' | '-' | '*' | '/' | '%' ;\
    sexpr: '(' <expr>* ')' ;\
    qexpr: '{' <expr>* '}' ;\
    expr: <numeral> | <operator> | <sexpr> | <qexpr> ;\
    lispy: /^/ <expr>* /$/ ;",
    Numeral, Operator, Sexpr, Qexpr, Expr, Lispy);
}

mpc_parser_t* init_grammar() {
    Numeral = mpc_new("numeral");
    Operator = mpc_new("operator");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("sexpr");
    Expr = mpc_new("expr");
    Lispy = mpc_new("lispy");
    define_grammar(Numeral, Operator, Sexpr, Qexpr, Expr, Lispy);
    return Lispy;
}

void cleanup() {
    mpc_cleanup(5, Numeral, Operator, Sexpr, Expr, Lispy);
}