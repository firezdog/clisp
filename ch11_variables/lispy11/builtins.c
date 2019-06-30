#include "lispy.h"

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
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "First argument of <define> should be a list.")
    lval* symbols = a->cell[0];
    for (int i = 0; i < symbols->cell_count; i++) {
        LASSERT(symbols->cell[i], symbols->cell[i]->type == LVAL_OP, "First argument of <define> is not a list of symbols.");
    }
    int nSyms = symbols->cell_count;
    int argumentParity = nSyms + 1 == a->cell_count;
    LASSERT(a, argumentParity, "<define>: list of definiens and series of definienda must be of equal length.");
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
    BUILTIN_ARG_CHECK(a, 2, "cons", 2, a->cell_count);
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR, "<cons> requires a value followed by an expression in curly brackets (q-expression).");
    lval* x = lval_qexpr();
    x = lval_add(x, a->cell[0]);
    return lval_join(x, a->cell[1]);
}

lval* builtin_init(lenv* e, lval* a) {
    /* to return everything but the last, get the length of a, n, pop off n-1, assuming zero indexing, and return.  edge case -- must not be empty -- so that init of a singleton is the empty list. */
    BUILTIN_ARG_CHECK(a, 1, "init", 1, a->cell_count);
    BUILTIN_EMPTY_CHECK(a, "init");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "<init> only accepts expressions in curly brackets (q-expressions).");
    int n = a->cell[0]->cell_count;
    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v,n-1));
    return v;
}

lval* builtin_len(lenv* e, lval* a) {
    BUILTIN_ARG_CHECK(a, 1, "len", 1, a->cell_count);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "<len> only accepts expressions in curly brackets (q-expressions).");
    int n = a->cell[0]->cell_count;
    return lval_num(n);
}

lval* builtin_head(lenv* e, lval* a) {
    /*Check error conditions*/
    BUILTIN_ARG_CHECK(a, 1, "head", 1, a->cell_count);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "<head> only accepts expressions in curly brackets (q-expressions).");
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
    BUILTIN_ARG_CHECK(a, 1, "tail", 1, a->cell_count);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "<tail> only accepts expressions in curly brackets (q-expressions).");
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
    BUILTIN_ARG_CHECK(a, 1, "eval", 1, a->cell_count);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "<eval> only accepts expressions in curly brackets (q-expressions).")
    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->cell_count; i++) {
        // (1) are all args QEXPR?
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "<join> only accepts expressions in curly brackets (q-expressions).")
         // join args one by one using lval_join
    }
    lval* x = lval_pop(a, 0);
    while (a->cell_count) {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
}
