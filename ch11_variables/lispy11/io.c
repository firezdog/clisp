#include "lispy.h"

void lval_print(lenv* e, lval* v) {
    switch(v->type) {
        case(LVAL_NUM)      :   printf("%g", v->num);  break;
        case(LVAL_ERR)      :   printf("Error: %s", v->err); break;
        case(LVAL_OP)       :   printf("%s", v->op); break; 
        case(LVAL_FN)       :
            for (int i = 0; i < e->count; i++) {
                if (e->assignments[i]->fn == v->fn) {
                    printf("<function>: %s", e->variables[i]); break;
                }
            }
            break;
        case(LVAL_SEXPR)    :   lval_sexpr_print(e, v, '(', ')'); break;
        case(LVAL_QEXPR)    :   lval_sexpr_print(e, v, '{', '}'); break; 
    }
}

void lval_sexpr_print(lenv* e, lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->cell_count; i++) {
        lval_print(e, v->cell[i]);
        if (i != v->cell_count-1) { putchar(' '); }
    }
    putchar(close);
}

void lval_println(lenv* e, lval* v) { lval_print(e, v); putchar('\n'); }
#pragma endregion

#pragma region read
lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = strtold(t->contents, NULL);
    return errno != ERANGE ? 
        lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {
    /* If operator (symbol) or numeral, convert to type
    (working at the level of expr in the grammar) */
    if (strstr(t->tag, "numeral")) { return lval_read_num(t); }
    if (strstr(t->tag, "operator")) { return lval_op(t->contents); }
    /* If root (>) or sexpr create empty list and fill with contents. */
    lval* x = NULL;
    if (strstr(t->tag, "sexpr") || !strcmp(t->tag, ">")) 
    { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }
    for (int i = 0; i < t->children_num; i++) {
        // first, ignore parens and regex
        if (!(strcmp(t->children[i]->contents, "(") &&
            strcmp(t->children[i]->contents, ")") &&
            strcmp(t->children[i]->contents, "{") &&
            strcmp(t->children[i]->contents, "}") &&
            strcmp(t->children[i]->tag, "regex")))
            { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}

// enum => name of enum
char* return_type(int t) {
    switch(t) {
        case(LVAL_ERR): return "error";
        case(LVAL_FN): return "function";
        case(LVAL_NUM): return "number";
        case(LVAL_OP): return "operator / symbol";
        case(LVAL_QEXPR): return "quoted expression";
        case(LVAL_SEXPR): return "expression";
        default: return "unknown";
    }
}