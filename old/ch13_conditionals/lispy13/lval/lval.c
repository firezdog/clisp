#include "../lispy.h"

#define LVAL_STRCPY(x, v, field) \
    x->field = malloc(strlen(v->field) + 1); \
    strcpy(x->field, v->field);

#pragma region constructors
lval* lval_fn(lbuiltin fn) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FN;
    v->builtin = fn;
    return v;
}

lval* lval_lambda(lval* formals, lval* body) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FN;
    v->builtin = NULL;
    v->env = lenv_new();
    v->formals = formals;
    v->body = body;
    return v;
}

lval* lval_num(double x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    v->builtin = NULL;
    return v;
}

lval* lval_err(char* format, ...) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    va_list args;
    va_start(args, format);
    v->err = malloc(512);
    vsnprintf(v->err, 511, format, args);
    v->err = realloc(v->err, strlen(v->err) + 1);
    va_end(args);
    v->builtin = NULL;
    return v;
}

lval* lval_op(char* op) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_OP;
    v->op = malloc(strlen(op) + 1);
    strcpy(v->op, op);
    v->builtin = NULL;
    return v;
}

lval* lval_sexpr() {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->cell_count = 0;
    v->cell = NULL;
    v->builtin = NULL;
    return v;
}

lval* lval_qexpr() {
    lval* v = lval_sexpr();
    v->type = LVAL_QEXPR;
    v->builtin = NULL;
    return v;
}

lval* lval_str(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_STR;
    v->str = malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

lval* lval_to_bool(lval* evaluandum) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_BOOL;
    switch(evaluandum->type) {
        // truthiness
        case LVAL_FN:
        case LVAL_OP:
        case LVAL_ERR:
            v->truth_value = LVAL_TRUE;
            break;
        case LVAL_NUM:
            v->truth_value = evaluandum->num == 0 ? LVAL_FALSE : LVAL_TRUE;
            break;
        case LVAL_QEXPR:
        case LVAL_SEXPR:    // in fact this could never be the value, because it would be evaluated?
            v->truth_value = evaluandum->cell_count > 0 ? LVAL_TRUE : LVAL_FALSE;
            break;
        case LVAL_BOOL:
            v->truth_value = evaluandum->truth_value;
    }
    return v;
}
#pragma endregion

#pragma region helpers
// copy an lval.
lval* lval_copy(lval* v) {
    lval* x = malloc(sizeof(lval));
    x->type = v->type;
    switch(v->type) {
        case LVAL_FN: 
            if (!v->builtin) {
                x->builtin = NULL;
                x->env = lenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            } else {
                x->builtin = v->builtin; break;
            }
            break;
        case LVAL_STR:
            x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;
        case LVAL_NUM: x->num = v->num; break;
        case LVAL_ERR: LVAL_STRCPY(x, v, err); break;
        case LVAL_OP: LVAL_STRCPY(x, v, op); break;
        case LVAL_QEXPR: 
        case LVAL_SEXPR:
            x->cell_count = v->cell_count;
            x->cell = malloc(sizeof(lval*) * x->cell_count);
            for (int i = 0; i < x->cell_count; i++) {
                // we need the recursive call here to copy all contents
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
    }
    return x;
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
        case LVAL_STR: free(v->str); break;
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_OP: free(v->op); break;
        case LVAL_FN: 
            if (!v->builtin) {
                lenv_del(v->env);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;
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