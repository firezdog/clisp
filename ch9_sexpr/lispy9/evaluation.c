#include "lispy.h"
#include <string.h>

lval* lval_eval_sexpr(lval* v) {
    // evaluate all the children with lval_eval
    for (int i = 0; i < v->cell_count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }
    // check for and return first error (lval_take)
    for (int i = 0; i < v->cell_count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v,i); }
    }
    // return lval* with no children
    if (v->cell_count == 0) { return v; }
    // return singleton in parens e.g. (5)
    if (v->cell_count == 1) { return lval_take(v, 0); }

    // separate first element with lval_pop
    lval* f = lval_pop(v, 0);
    // if it is not a symbol, delete it and other values, return error
    if (f->type != LVAL_OP) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression must start with a symbol.");
    }
    // if it is a symbol, check it and pass it to builtin_op to calculate
    lval* result = builtin_op(v, f->op);
    lval_del(f);
    return result;
}

// evaluates the child of an sexpr based on whether or not it itself is an sexpr
lval* eval(lval* v) {
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
    return v;
}

// take item from cell at index and shift left, returning item
lval* lval_pop(lval* v, int i) {
    lval* p = v->cell[i];
    lval_del(v->cell[i]);
    for (i; i < v->cell_count-1; i++) {
        v->cell[i] = v->cell[i+1];
    }
}

// take item from cell at index and delete cell, returning item
lval* lval_take(lval* v, int i);