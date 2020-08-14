#include "lispy.h"
#include <string.h>

lval eval_op(lval a, char* op, lval b) {
    // doesn't cover the case in which both are errors
    if (a.type == LVAL_ERR) { return a; }
    if (b.type == LVAL_ERR) { return b; }
    // using ! for falsy 0 (unintitive and prob less legible but more concise)
    if (!strcmp(op, "+")) { return lval_num(a.num + b.num); }
    if (!strcmp(op, "-")) { return lval_num(a.num - b.num); }
    if (!strcmp(op, "*")) { return lval_num(a.num * b.num); }
    // c language doesn't have exceptions, so I don't know how to handle case where b is 0
    if (!strcmp(op, "/") && b.num != 0) { return lval_num(a.num / b.num); }
    if (!strcmp(op, "/") && b.num == 0) { return lval_err(LERR_DIV_ZERO); }
}

lval evaluate(mpc_ast_t* t) {
    // apparently 0 is also "falsy" in C?
    if (strstr(t->tag, "numeral")) {
        errno = 0;
        int x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_NUM);
    } // else un-necessary b/c of return statement above
    
    /* first node is an expr? -- 2nd child of expr is always op */
    char* op = t->children[1]->contents;

    /* left expression */
    lval x = evaluate(t->children[2]);

    /* go until you reach ')'?  -- I just don't find this intuitive -- I guess it allows for any number of arguments to flank an operator */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, evaluate(t->children[i]));
        i++;
    }

    return x;
}