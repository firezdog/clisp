#include "lispy.h"

void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* String, mpc_parser_t* Operator, mpc_parser_t* Sexpr, mpc_parser_t* Qexpr, mpc_parser_t* Expr, mpc_parser_t* Lispy) {
    mpca_lang(MPCA_LANG_DEFAULT,
    "numeral: /-?([0-9]*\\.)?[0-9]+/ ;\
    string: /\"(\\\\.|[^\"])*\"/ ;\
    operator: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%?]+/ ;\
    sexpr: '(' <expr>* ')' ;\
    qexpr: '{' <expr>* '}' ;\
    expr: <numeral> | <operator> | <sexpr> | <qexpr> | <string> ;\
    lispy: /^/ <expr>* /$/ ;",
    Numeral, String, Operator, Sexpr, Qexpr, Expr, Lispy);
}

void init_grammar() {
    Numeral = mpc_new("numeral");
    String = mpc_new("string");
    Operator = mpc_new("operator");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    Lispy = mpc_new("lispy");
    define_grammar(Numeral, String, Operator, Sexpr, Qexpr, Expr, Lispy);
}

void parse(lenv* e) {
    mpc_result_t r;    
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
        // Load AST from output
        mpc_ast_t* a = r.output;
        lval* analysis = lval_read(r.output);
        // analysis is deleted in the course of lval_eval
        lval* result = lval_eval(e, analysis);
        lval_println(e, result);
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