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

#pragma region macros
#define LASSERT(args, cond, err, ...) \
    if(!(cond)) { lval_del(args); return lval_err(err, ##__VA_ARGS__); }

// as is this seems repetitive -- wish num_args could be used along with item->cell_count in the string.
#define BUILTIN_ARG_CHECK(item, num_args, ...) \
    if(!(item->cell_count == num_args)) { lval_del(item); return lval_err("<%s> only accepts %d argument(s) but was passed %d argument(s)", ##__VA_ARGS__); }
// this could be generalized to a general check that list is of length n

#define BUILTIN_EMPTY_CHECK(item, ...) \
    if(item->cell[0]->cell_count == 0) { lval_del(item); return lval_err("%s does not accept an empty quote.", ##__VA_ARGS__); }
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
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);
struct lval {
    int type;
    double num;
    char* err;
    char* op;
    lbuiltin fn;
    struct lval** cell;
    int cell_count;
};
lval* lval_fn(lbuiltin fn);
lval* lval_num(double x);
lval* lval_err(char* format, ...);
lval* lval_op(char* op);
lval* lval_sexpr();
lval* lval_qexpr();
lval* lval_copy(lval* v);
lval* lval_add(lval* x, lval* v);
void lval_del(lval* v);
void delete_cells(lval* v);
#pragma endregion

#pragma region lenv
struct lenv {
    int count;
    char** variables;
    lval** assignments;
};
lenv* lenv_new(void);
void lenv_del(lenv* v);
void lenv_put(lenv* e, lval* var, lval* asgn);
lval* lenv_get(lenv* e, lval* var);
void lenv_add_builtin(lenv* e, char* name, lbuiltin fn);
void lenv_add_builtins(lenv* e);
#pragma endregion

#pragma region io
void lval_print(lval* v);
void lval_sexpr_print(lval* v, char open, char close);
void lval_println(lval* v);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
#pragma endregion

#pragma region evaluation
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);
lval* lval_take(lval* v, int i);
lval* lval_pop(lval* v, int i);
int check_div_0(double x);
#pragma endregion

#pragma region builtins
lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_define(lenv* e, lval* a);
lval* builtin_subtract(lenv* e, lval* a);
lval* builtin_divide(lenv* e, lval* a);
lval* builtin_multiply(lenv* e, lval* a);
lval* builtin_modulus(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_init(lenv* e, lval* a);
lval* builtin_len(lenv* e, lval* a);
lval* lval_join(lval* x, lval* y);
lval* builtin(lenv* e, lval* a, char* func);
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
void init_grammar();
void parse(lenv* e);
void cleanup();
#pragma endregion

#endif