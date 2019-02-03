#include "lispy.h"

#pragma region constructors
lval* lval_num(double x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_err(char* message) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(message) + 1);
    strcpy(v->err, message);
    return v;
}

lval* lval_op(char* op) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_OP;
    v->op = malloc(strlen(op) + 1);
    strcpy(v->op, op);
    return v;
}

lval* lval_sexpr() {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->cell_count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_qexpr() {
    lval* v = lval_sexpr();
    v->type = LVAL_QEXPR;
    return v;
}

// add lval* v to lval* x->cell
lval* lval_add(lval* x, lval* v) {
    if (!(x->type == LVAL_SEXPR || x->type == LVAL_QEXPR)) { 
        puts("Tried to add a cell to a non SEXPR or QEXPR"); 
        return NULL; 
    }
    x->cell_count++;
    x->cell = realloc(x->cell, x->cell_count * sizeof(lval*));
    x->cell[x->cell_count-1] = v;
    return x;
}

void lval_del(lval* v) {
    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_OP: free(v->op); break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            delete_cells(v);
            break;
    }
    free(v);
}

void delete_cells(lval* v) {
    for (int i = 0; i < v->cell_count; i++) {
        lval_del(v->cell[i]);
    }
    free(v->cell);
}

#pragma endregion

#pragma region print
void lval_print(lval* v) {
    switch(v->type) {
        case(LVAL_NUM)  :   printf("%g", v->num);  break;
        case(LVAL_ERR)  :   printf("Error: %s", v->err); break;
        case(LVAL_OP)   :   printf("%s", v->op); break; 
        case(LVAL_SEXPR):   lval_sexpr_print(v, '(', ')'); break;
        case(LVAL_QEXPR):   lval_sexpr_print(v, '{', '}'); break; 
    }
}

void lval_sexpr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->cell_count; i++) {
        lval_print(v->cell[i]);
        if (i != v->cell_count-1) { putchar(' '); }
    }
    putchar(close);
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }
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
#pragma endregion