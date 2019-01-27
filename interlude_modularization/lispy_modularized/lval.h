#ifndef LVAL_INCLUDED
#define LVAL_INCLUDED

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

#endif