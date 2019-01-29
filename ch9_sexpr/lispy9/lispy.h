#ifndef LISPY_INCLUDED
#define LISPY_INCLUDED

#include "mpc_library/mpc.h"

#pragma region lval
enum { LVAL_NUM, LVAL_ERR, LVAL_OP, LVAL_SEXPR, LERR_NUM, LERR_OP, LERR_DIV_ZERO };

typedef struct lval {
    int type;
    int num;
    char* err;
    char* op;
    struct lval** cell;
    int cell_count;
} lval;

lval* lval_num(int x);
lval* lval_err(char* mesage);
lval* lval_op(char* op);
lval* lval_sexpr();
lval* lval_add(lval* x, lval* v);
void lval_del(lval* v);

void lval_print(lval* v);
void lval_sexpr_print(lval* v, char open, char close);
void lval_println(lval* v);

lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
#pragma endregion

#pragma region evaluation
lval* lval_eval_sexpr(lval* v);
lval* eval(lval* v);
lval* lval_take(lval* v, int i);
lval* lval_pop(lval* v, int i);
#pragma endregion

#pragma region grammar
void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* Operator, mpc_parser_t* Sexpr, mpc_parser_t* Expr, mpc_parser_t* Lispy);
#pragma endregion

#endif