#include "lispy.h"

lval* lval_eval_sexpr(lenv* e, lval* v) {
    // evaluate all the children with lval_eval
    for (int i = 0; i < v->cell_count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
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
    if (f->type != LVAL_FN) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression must start with a function.");
    }
    // if it is a symbol, run its callback on v to evaluate
    lval* result;
    result = lval_call(e, f, v); 
    return result;
}

// evaluates the child of an sexpr based on whether or not it itself is an sexpr
lval* lval_eval(lenv* e, lval* v) {
    if (v->type == LVAL_OP) { 
        lval* x = lenv_get(e, v); 
        lval_del(v); 
        return x; 
    }
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
    return v;
}

// take item from cell at index and shift left, returning item
lval* lval_pop(lval* v, int i) {
    lval* p = v->cell[i];
    // for the (arg 3) n (= cell_count - i - 1) items to the right of the member before the i'th element (which remains after the i'th element is popped), copy them from the position (arg 2) i+1 to the position (arg 1) i.
    memmove(&v->cell[i], &v->cell[i+1],
        sizeof(lval*) * (v->cell_count-i-1));
    // shrink the cell count and the memory block
    v->cell_count--;
    v->cell = realloc(v->cell, sizeof(lval*) * v->cell_count);
    return p;
}

// take item from cell at index and delete cell, returning item
lval* lval_take(lval* v, int i) {
    // we pop it so that it doesn't get deleted in the next line.
    lval* t = lval_pop(v, i);
    lval_del(v);
    return t;
}

// pop each item from y and add it to x until y is empty, then return x.
lval* lval_join(lval* x, lval* y) {
    while (y->cell_count) {
        x = lval_add(x, lval_pop(y,0));
    }
    lval_del(y);
    return x;
}

int check_div_0(double x) {
    return x == 0;
}