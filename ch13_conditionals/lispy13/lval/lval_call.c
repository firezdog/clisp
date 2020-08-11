#include "../lispy.h"

lval* get_next_param(int args_required, lval* f) 
{
    // handle 0 param case and get next param
    lval* dummy_param = lval_qexpr();
    dummy_param->op = "NULL";
    return args_required == 0 ?
        dummy_param :
        lval_pop(f->formals, 0);
}

lval* eval_params(
    int* args_required, 
    int* args_remaining, 
    lenv* e, 
    lval* f, 
    lval* a
)
{
    while (*args_remaining) {
        lval* param = get_next_param(*args_required, f);

        // next param is &: pop & and param list, then fill the function
        if (!strcmp(param->op, "&")) {
            LASSERT(
                a, 
                *args_required == 1, 
                "Function format invalid: '&' symbol must be followed by one argument naming the list of variable arguments"
            );
            lval* arg_list_param = lval_pop(f->formals, 0);
            lenv_put(e, arg_list_param, builtin_list(e, a));
            lval_del(arg_list_param); 
            break;
        } else {
        // otherwise pop next arg and fill param in function
            lval* arg = lval_pop(a, 0);
            lenv_put(f->env, param, arg);
            lval_del(arg);
        }

        lval_del(param);
    }
    return NULL;
}

lval* lval_call(lenv* e, lval* f, lval* a) {
    /* Given a function, an lval with assignments, and an environment, call the function with those assignments in that environment.*/
    if (f->builtin) { return f->builtin(e, a); }
    int* args_required = &(f->formals->cell_count);
    int* args_supplied = &(a->cell_count);

    lval* error;
    
    error = eval_params(args_required, args_supplied, e, f, a);
    if (error != NULL) return error; 
    lval_del(a);

    // return evaluation
    f->env->parent_environment = e;
    if (*args_required == 0) {
        return builtin_eval(
            f->env, 
            lval_add(
                lval_sexpr(), 
                lval_copy(f->body)
            )
        );
    }

    // return partial evaluation
    return lval_copy(f);
}