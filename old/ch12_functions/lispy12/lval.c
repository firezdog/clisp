#include "lispy.h"
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
#pragma endregion

#pragma region helpers
lval* lval_call(lenv* e, lval* f, lval* a) {
/* Given a function, an lval with assignments, and an environment, call the function with those assignments in that environment.*/
// This talk of "calling a function" is slightly misleading -- what we're really doing is generating a complete s-expression from the values given and the function schema, then evaluating the schema.  So if the function is {+ x y} and the values are (3, 4), we generate {+ 3 4} and evalute it, returning (7).    
    if (f->builtin) { return f->builtin(e, a); }
    // how many arguments are required and how many were supplied? -- used for error handling only
    int args_required = f->formals->cell_count;
    int args_supplied = a->cell_count;
    // repeatedly evaluate function by applying arguments in order
    while (a->cell_count) {
        // case: the function is completely bound but there are still arguments in the queue
        if (f->formals->cell_count == 0) {
            lval_del(a);
            return lval_err("Function passed too many arguments: got %s, expected %s", args_supplied, args_required);
        }
        lval* param = lval_pop(f->formals, 0);
        // handle extra arguments -- not the symbol following and is the last symbol that can be defined
        if (!strcmp(param->op, "&")) {
            LASSERT(a, f->formals->cell_count == 1, "Function format invalid: '&' symbol must be followed by one argument naming the list of variable arguments");
            lval* arg_list_param = lval_pop(f->formals, 0);
            lenv_put(e, arg_list_param, builtin_list(e, a));
            lval_del(arg_list_param); 
            break;
        } else {
            // a->cell_count decreases by one
            lval* arg = lval_pop(a, 0);
            lenv_put(f->env, param, arg);
            lval_del(arg);
        }
        lval_del(param);
    }
    lval_del(a);
    // only evaluate if all formals have been bound
    if (f->formals->cell_count > 0 && !strcmp(f->formals->cell[0]->op, "&")) {
        lval_del(lval_pop(f->formals, 0));
        LASSERT(NULL, f->formals->cell_count == 1,  "Function format invalid: '&' symbol must be followed by one argument naming the list of variable arguments");
        lval* param = lval_pop(f->formals, 0);
        // declare it so we can then cleanup below.
        lval* arg = lval_qexpr();
        lenv_put(f->env, param, arg);
        lval_del(param); lval_del(arg);
    }
    f->env->parent_environment = e;
    if (f->formals->cell_count == 0) {
        // evaluate the function (an expression) using the variables in its environment and then parent environments
        return builtin_eval(
            f->env, 
            lval_add(
                lval_sexpr(), 
                lval_copy(f->body)
            )
        );
    }
    return lval_copy(f);
}

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