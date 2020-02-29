#include "../lispy.h"

lval* lval_call(lenv* e, lval* f, lval* a) {
    /* Given a function, an lval with assignments, and an environment, call the function with those assignments in that environment.*/
    if (f->builtin) { return f->builtin(e, a); }
    int* args_required = &(f->formals->cell_count);
    int* args_supplied = &(a->cell_count);
    while (*args_supplied) {
        int supplied = *args_supplied; int required = *args_required;
        LASSERT(
            a, 
            required != 0, 
            "Error: function doesn't take any arguments.", 
            supplied, 
            required
        )
        lval* param = lval_pop(f->formals, 0);
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
            if (*args_required + 1 != a->cell_count) 
                return lval_err(
                    "Too many arguments passed to anonymous function. Expected %i, got %i.",
                    *args_required + 1,
                    *args_supplied
                );
            lval* arg = lval_pop(a, 0);
            lenv_put(f->env, param, arg);
            lval_del(arg);
        }
        lval_del(param);
    }
    lval_del(a);
    if (*args_required > 0 && !strcmp(f->formals->cell[0]->op, "&")) {
        lval_del(lval_pop(f->formals, 0));
        LASSERT(NULL, f->formals->cell_count == 1,  "Function format invalid: '&' symbol must be followed by one argument naming the list of variable arguments");
        lval* param = lval_pop(f->formals, 0);
        lval* arg = lval_qexpr();
        lenv_put(f->env, param, arg);
        lval_del(param); lval_del(arg);
    }
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
    return lval_copy(f);
}
