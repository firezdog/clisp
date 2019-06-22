#ifndef LISPY_INCLUDED
#define LISPY_INCLUDED

#include "mpc_library/mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma region history
#if defined(__CYGWIN__) | defined(_WIN32)

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    // malloc = memory allocation for cpy variable to be as large as the buffer + 1
    char* cpy = malloc(strlen(buffer)+1);
    // copy buffer to cpy
    strcpy(cpy, buffer);
    // format cpy to end with 0 (end of string character?)
    cpy[strlen(cpy)-1] = '\0';
    // return string
    return cpy;
}

/* Fake add_history function -- not needed in windows implementation */
void add_history(char* unused) {}

#else
// compile with flag "-ledit"
// #include <editline/history.h> // not needed on mac
#include <editline/readline.h>

#endif
#pragma endregion

#pragma region lval
enum { 
    LVAL_FN,
    LVAL_NUM, 
    LVAL_ERR, 
    LVAL_OP, 
    LVAL_QEXPR,
    LVAL_SEXPR, 
    LERR_NUM, 
    LERR_OP, 
    LERR_DIV_ZERO };

typedef struct lval {
    int type;
    double num;
    char* err;
    char* op;
    lbuiltin fn;
    struct lval** cell;
    int cell_count;
} lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

lval* lval_num(double x);
lval* lval_err(char* mesage);
lval* lval_op(char* op);
lval* lval_sexpr();
lval* lval_qexpr();
lval* lval_add(lval* x, lval* v);
void lval_del(lval* v);
void delete_cells(lval* v);

void lval_print(lval* v);
void lval_sexpr_print(lval* v, char open, char close);
void lval_println(lval* v);

lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
#pragma endregion

#pragma region evaluation
lval* lval_eval_sexpr(lval* v);
lval* lval_eval(lval* v);
lval* lval_take(lval* v, int i);
lval* lval_pop(lval* v, int i);
lval* builtin_op(lval* a, char* op);
lval* builtin_cons(lval* a);
lval* builtin_head(lval* a);
lval* builtin_tail(lval* a);
lval* builtin_list(lval* a);
lval* builtin_eval(lval* a);
lval* builtin_join(lval* a);
lval* builtin_init(lval* a);
lval* builtin_len(lval* a);
lval* lval_join(lval* x, lval* y);
lval* builtin(lval* a, char* func);
int check_div_0(double x);
#pragma endregion

#pragma region grammar
char* input;
char buffer[2048];
mpc_parser_t* Numeral;
mpc_parser_t* Operator;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;
void define_grammar(mpc_parser_t* Numeral, mpc_parser_t* Operator, mpc_parser_t* Sexpr, mpc_parser_t* Qexpr, mpc_parser_t* Expr, mpc_parser_t* Lispy);
mpc_parser_t* init_grammar();
void parse();
void cleanup();
#pragma endregion

#endif