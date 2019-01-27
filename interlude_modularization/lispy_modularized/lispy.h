#ifndef LISPY_INCLUDED
#define LISPY_INCLUDED

#include "mpc_library/mpc.h"

// lval
enum { LVAL_NUM, LVAL_ERR, LERR_NUM, LERR_OP, LERR_DIV_ZERO };

typedef struct {
    int type;
    int num;
    int err;
} lval;

lval lval_num(int x);
lval lval_err(int x);

void lval_print(lval v);
void lval_println(lval v);

// evaluation 
lval evaluate(mpc_ast_t* t);
lval eval_op(lval a, char* op, lval b);

// grammar
void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* Operator, mpc_parser_t* Expr, mpc_parser_t* Lispy);

#endif