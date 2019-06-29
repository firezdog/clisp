#include "lispy.h"

#define LASSERT(args, cond, err) \
    if(!(cond)) { lval_del(args); return lval_err(err); }

#define BUILTIN_ARG_CHECK(item, num_args, function) \
    if(!(item->cell_count == num_args)) { lval_del(item); return lval_err(function " only accepts " #num_args " argument(s)"); }

// this could be generalized to a general check that list is of length n
#define BUILTIN_EMPTY_CHECK(item, function) \
    if(item->cell[0]->cell_count == 0) { lval_del(item); return lval_err(function " does not accept an empty quote."); }

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
    lval* result = f->fn(e, v);
    lval_del(f);
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

// I don't think this is getting used anymore? builtin_op is, but not this.
// lval* builtin(lenv* e, lval* a, char* func) {
//     if (!strcmp("cons", func)) return builtin_cons(e, a);
//     if (!strcmp("init", func)) return builtin_init(e, a);
//     if (!strcmp("len", func)) return builtin_len(e, a);
//     if (!strcmp("head", func)) return builtin_head(e, a);
//     if (!strcmp("tail", func)) return builtin_tail(e, a);
//     if (!strcmp("list", func)) return builtin_list(e, a);
//     if (!strcmp("eval", func)) return builtin_eval(e, a);
//     if (!strcmp("join", func)) return builtin_join(e, a);
//     if (strstr("+-*/%", func)) return builtin_op(e, a, func);
//     lval_del(a);
//     return lval_err("Unknown function.");
// }

// requirement: return error if any input is not a number.
lval* builtin_op(lenv* e, lval* a, char* op) {
    // 1. check that all arguments are numbers.
    for (int i = 0; i < a->cell_count; i++) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_err("Attempting to operate on non-numeral.");
        }
    }
    // 2. pop first argument.
    lval* result = lval_pop(a, 0);
    // 3. If no more and op is -, return negation.
    if ((a->cell_count == 0) && !strcmp(op, "-")) 
    { result->num = -result->num; }
    // 4. If more, pop the next and perform arithmetic with the op until done.
    while (a->cell_count > 0) {
        lval* y = lval_pop(a, 0);
        if (!strcmp(op, "*")) { result->num *= y->num; }
        if (!strcmp(op, "+")) { result->num += y->num; }
        if (!strcmp(op, "-")) { result->num -= y->num; }
        // But if we have a 0 for division, delete pops and arguments, return error.'
        if (!strcmp(op, "/")) {
            if (check_div_0(y->num)) { return lval_err("Division by zero."); }
            result->num /= y->num;
        }
        // or modulus
        if (!strcmp(op, "%")) {
            if (check_div_0(y->num)) { return lval_err("Division by zero."); }
            result->num = (int) result->num % (int) y->num;
        }
        // we're done with y
        lval_del(y);
    }
    lval_del(a); return result;
}

// seems like we could use macros here also.

// remember this is taking an s-expression consisting of a q-expression and a series of s-expressions of length equal to the length of the q-expression
lval* builtin_define(lenv* e, lval* a){
    // the arguments are the cells of a
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "First argument should be a list.")
    lval* symbols = a->cell[0];
    for (int i = 0; i < symbols->cell_count; i++) {
        LASSERT(symbols->cell[i], symbols->cell[i]->type == LVAL_OP, "First argument is not a list of symbols.");
    }
    int nSyms = symbols->cell_count;
    int argumentParity = nSyms + 1 == a->cell_count;
    LASSERT(a, argumentParity, "List of definiens and series of definienda must be of equal length.");
    for (int i = 0; i < symbols->cell_count; i++) {
        lenv_put(e, symbols->cell[i], a->cell[i + 1]);
    }
    lval_del(a);
    // the empty expression
    return lval_sexpr();
}

lval* builtin_add(lenv* e, lval* a) {
    return builtin_op(e, a, "+");
}

lval* builtin_multiply(lenv* e, lval* a) {
    return builtin_op(e, a, "*");
}

lval* builtin_subtract(lenv* e, lval* a) {
    return builtin_op(e, a, "-");
}

lval* builtin_divide(lenv* e, lval* a) {
    return builtin_op(e, a, "/");
}

lval* builtin_modulus(lenv* e, lval* a) {
    return builtin_op(e, a, "%");
}

lval* builtin_cons(lenv* e, lval* a) {
    BUILTIN_ARG_CHECK(a, 2, "cons");
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR, "Function 'cons' requires a value followed by an expression in curly brackets (q-expression).");
    lval* x = lval_qexpr();
    x = lval_add(x, a->cell[0]);
    return lval_join(x, a->cell[1]);
}

lval* builtin_init(lenv* e, lval* a) {
    /* to return everything but the last, get the length of a, n, pop off n-1, assuming zero indexing, and return.  edge case -- must not be empty -- so that init of a singleton is the empty list. */
    BUILTIN_ARG_CHECK(a, 1, "init");
    BUILTIN_EMPTY_CHECK(a, "init");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'init' only accepts expressions in curly brackets (q-expressions).");
    int n = a->cell[0]->cell_count;
    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v,n-1));
    return v;
}

lval* builtin_len(lenv* e, lval* a) {
    BUILTIN_ARG_CHECK(a, 1, "len");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'len' only accepts expressions in curly brackets (q-expressions).");
    int n = a->cell[0]->cell_count;
    return lval_num(n);
}

lval* builtin_head(lenv* e, lval* a) {
    /*Check error conditions*/
    BUILTIN_ARG_CHECK(a, 1, "head");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'head' only accepts expressions in curly brackets (q-expressions).");
    BUILTIN_EMPTY_CHECK(a, "head");
    lval* v = lval_take(a, 0);
    // repeatedly pop and delete item at v->cell[1] until list only has head.
    while(v->cell_count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval* builtin_tail(lenv* e, lval* a) {
    /*Check error conditions*/
    BUILTIN_ARG_CHECK(a, 1, "tail");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'tail' only accepts expressions in curly brackets (q-expressions).");
    BUILTIN_EMPTY_CHECK(a, "tail");
    // pop the first value off of v.
    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v,0));
    return v;
}

// convert lval* a to a q_expr.
lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_eval(lenv* e, lval* a) {
    BUILTIN_ARG_CHECK(a, 1, "eval");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' only accepts expressions in curly brackets (q-expressions).")
    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->cell_count; i++) {
        // (1) are all args QEXPR?
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' only accepts expressions in curly brackets (q-expressions).")
         // join args one by one using lval_join
    }
    lval* x = lval_pop(a, 0);
    while (a->cell_count) {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
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
    return x == 0 ? 1 : 0;
}