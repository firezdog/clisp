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

lval* builtin_lambda(lenv* e, lval* a) {
    BUILTIN_ARG_CHECK(a, 2, "\\", 2, a->cell_count);
    BUILTIN_TYPE_CHECK(a, 0, "\\", LVAL_QEXPR);
    BUILTIN_TYPE_CHECK(a, 1, "\\", LVAL_QEXPR);
    lval* formals_arg = a->cell[0];
    for (int i = 0; i < formals_arg->cell_count; i++) {
        // i hope you can store enum entries as ints...
        int type = formals_arg->cell[i]->type;
        LASSERT(a, type == LVAL_OP, "Lambda formal must consist of symbols but instead contained %s", return_type(type));
    }
    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);
    return lval_lambda(formals, body);
}

// remember this is taking an s-expression consisting of a q-expression and a series of s-expressions of length equal to the length of the q-expression
lval* define_variable(lenv* e, lval* a, char* func){
    // the arguments are the cells of "a"
    BUILTIN_TYPE_CHECK(a, 0, "define", LVAL_QEXPR);
    lval* symbols = a->cell[0];
    for (int i = 0; i < symbols->cell_count; i++) {
        BUILTIN_TYPE_CHECK(symbols, i, "define", LVAL_OP);
        // oh no -- now defining a variable is an N^2 operation! (well technically N*M)
        char* symbol = symbols->cell[i]->op;
        for (int i = 0; i < e->count; i++) {
            if (!strcmp(e->variables[i], symbol) && e->assignments[i]->builtin != NULL) { 
                return lval_err("%s is a reserved word."); 
            }
        }
    }
    int nSyms = symbols->cell_count;
    int argumentParity = nSyms + 1 == a->cell_count;
    LASSERT(a, argumentParity, "<define>: list of definiens and series of definienda must be of equal length.");
    for (int i = 0; i < symbols->cell_count; i++) {
        if (!strcmp(func, "def")) root_lenv_put(e, symbols->cell[i], a->cell[i + 1]);
        if (!strcmp(func, "let")) lenv_put(e, symbols->cell[i], a->cell[i+1]);
    }
    lval_del(a);
    // the empty expression
    return lval_sexpr();
}

lval* builtin_let(lenv* e, lval* a) {
    return define_variable(e, a, "let");
}

lval* builtin_def(lenv* e, lval* a) {
    return define_variable(e, a, "def");
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
    BUILTIN_TYPE_CHECK(a, 1, "cons", LVAL_QEXPR);
    lval* x = lval_qexpr();
    x = lval_add(x, a->cell[0]);
    return lval_join(x, a->cell[1]);
}

lval* builtin_init(lenv* e, lval* a) {
    /* to return everything but the last, get the length of a, n, pop off n-1, assuming zero indexing, and return.  edge case -- must not be empty -- so that init of a singleton is the empty list. */
    BUILTIN_ARG_CHECK(a, 1, "init", 1, a->cell_count);
    BUILTIN_TYPE_CHECK(a, 0, "init", LVAL_QEXPR);
    BUILTIN_EMPTY_CHECK(a, "init");
    int n = a->cell[0]->cell_count;
    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v,n-1));
    return v;
}

lval* builtin_len(lenv* e, lval* a) {
    BUILTIN_ARG_CHECK(a, 1, "len", 1, a->cell_count);
    BUILTIN_TYPE_CHECK(a, 0, "len", LVAL_QEXPR);
    int n = a->cell[0]->cell_count;
    return lval_num(n);
}

lval* builtin_head(lenv* e, lval* a) {
    /*Check error conditions*/
    BUILTIN_ARG_CHECK(a, 1, "head", 1, a->cell_count);
    BUILTIN_TYPE_CHECK(a, 0, "head", LVAL_QEXPR);
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
    BUILTIN_TYPE_CHECK(a, 0, "tail", LVAL_QEXPR);
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
    BUILTIN_TYPE_CHECK(a, 0, "eval", LVAL_QEXPR);
    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->cell_count; i++) {
        // (1) are all args QEXPR?
        BUILTIN_TYPE_CHECK(a, i, "join", LVAL_QEXPR);
    }
    // join args one by one using lval_join
    lval* x = lval_pop(a, 0);
    while (a->cell_count) {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
}

lval* builtin_print_env(lenv* e, lval* v) {
    for (int i = 0; i < e->count; i++) {
        printf("%s: ", e->variables[i]);
        lval_println(e, e->assignments[i]);
    }
    return lval_sexpr();
}

lval* builtin_exit(lenv* e, lval* v) {
    run_lisp = 0;
    return lval_sexpr();
}

# define TWO_PLACE_RELATION(name, symbol, operator)\
lval* name(lenv* e, lval* a)\
{\
    BUILTIN_ARG_CHECK(a, 2, symbol, 2, a->cell_count);\
    BUILTIN_TYPE_CHECK(a, 0, symbol, LVAL_NUM);\
    BUILTIN_TYPE_CHECK(a, 1, symbol, LVAL_NUM);\
    lval* comp_a = lval_pop(a, 0);\
    lval* comp_b = lval_pop(a, 0);\
    lval_del(a);\
    double comp_a_num = comp_a->num;\
    double comp_b_num = comp_b->num;\
    lval_del(comp_a); lval_del(comp_b);\
    if (comp_a_num operator comp_b_num) return lval_num(1);\
    return lval_num(0);\
}

TWO_PLACE_RELATION(builtin_greater, ">", >)
TWO_PLACE_RELATION(builtin_lesser, "<", <)

// TODO: apply to non-numbers?
TWO_PLACE_RELATION(builtin_or, "or", ||);
TWO_PLACE_RELATION(builtin_and, "and", &&);

// (not x) returns 1 iff x is 0 
// TODO: how do you apply this to non-numerical expressions?
lval* builtin_not(lenv* e, lval* a)
{
    BUILTIN_ARG_CHECK(a, 1, "!", 1, a->cell_count);
    BUILTIN_TYPE_CHECK(a, 0, "!", LVAL_NUM);
    lval* value = lval_pop(a, 0);
    double value_num = value->num;
    lval_del(a);
    lval_del(value);
    if (value_num == 0) {
        return lval_num(1);
    }
    return lval_num(0);
}

// strategy for equals: a == b if (1) they have the same type and (2) their fields are equal (recursion)
lval* builtin_equals(lenv* e, lval* a)
{
    BUILTIN_ARG_CHECK(a, 2, "=", 2, a->cell_count);
    lval* comp_a = lval_pop(a, 0);
    lval* comp_b = lval_pop(a, 0);
    lval* result = lval_num(lvals_equal(comp_a, comp_b));
    lval_del(comp_a); lval_del(comp_b);
    return result;
}

int lvals_equal(lval* comp_a, lval* comp_b)
{
    if (comp_a->type != comp_b->type) return 0;
    
    switch (comp_a->type) 
    {
        case LVAL_NUM: return (comp_a->num == comp_b->num);
        case LVAL_ERR: return (!strcmp(comp_a->err, comp_b->err));
        // used for evaluating whether symbols in the body of an expression are the same
        case LVAL_OP: return (!strcmp(comp_a->op, comp_b->op));
        // this depends on equality being defined for q-expr
        case LVAL_FN:
            if (comp_a->builtin || comp_b->builtin) return (comp_a->builtin == comp_b->builtin);
            else return 
                lvals_equal(comp_a->formals, comp_b->formals) && 
                lvals_equal(comp_a->body, comp_b->body);
        break;
        // TODO: q-expr and s-expr
    }
    return 0;
}