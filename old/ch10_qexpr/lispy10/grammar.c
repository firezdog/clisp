#include "lispy.h"

void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* Operator, mpc_parser_t* Sexpr, mpc_parser_t* Qexpr, mpc_parser_t* Expr, mpc_parser_t* Lispy) {
    mpca_lang(MPCA_LANG_DEFAULT,
    "numeral: /-?([0-9]*\\.)?[0-9]+/ ;\
    operator:   \
                \"list\" | \"head\" | \"tail\" | \
                \"join\" | \"eval\" | \"len\" | \
                \"init\" | \"cons\" | \
                '+' | '-' | '*' | '/' | '%' ;\
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
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    Lispy = mpc_new("lispy");
    define_grammar(Numeral, Operator, Sexpr, Qexpr, Expr, Lispy);
    return Lispy;
}

void parse() {
    mpc_result_t r;    
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
        // Load AST from output
        mpc_ast_t* a = r.output;
        lval* analysis = lval_read(r.output);
        // analysis is deleted in the course of lval_eval
        lval* result = lval_eval(analysis);
        lval_println(result);
        lval_del(result);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.output);
        mpc_err_delete(r.output);
    }
}

void cleanup() {
    mpc_cleanup(6, Numeral, Operator, Sexpr, Qexpr, Expr, Lispy);
}