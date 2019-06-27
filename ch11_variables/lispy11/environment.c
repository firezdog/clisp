#include "lispy.h"

lenv* lenv_new(void) {
    lenv* v = malloc(sizeof(lenv));
    v->count = 0;
    v->variables = NULL;
    v->assignments = NULL;
    return v;
}

void lenv_del(lenv* v) {
    /* iterate over variables and assignments and delete / free them */
    for (int i = 0; i < v->count; i++) {
        free(v->variables[i]);
        lval_del(v->assignments[i]);
    }
    free(v->variables);
    free(v->assignments);
    free(v);
}

void lenv_put(lenv* e, lval* var, lval* asgn) {
    /* puts a new value in the environment dictionary.  First, loop through and check if it is in the dictionary.  If so, delete the corresponding value and put the new one in.  If not, add it to the variables list, then add the corresponding assignment. 
    
    Note: here we put a copy in rather than the processed input itself -- presumably because the input is always getting deleted after the end of processing?
    */
    for (int i = 0; i < e->count; i++) {
        if (!strcmp(e->variables[i], var->op)) { 
            lval_del(e->assignments[i]);
            e->assignments[i] = lval_copy(asgn);
            return; 
        }
    }
    int n = ++e->count;
    e->variables = realloc(e->variables, sizeof(char*) * n);
    e->assignments = realloc(e->assignments, sizeof(lval*) * n);
    e->variables[n - 1] = malloc(strlen(var->op) + 1);
    strcpy(e->variables[n - 1], var->op);
    e->assignments[n - 1] = lval_copy(asgn);
}

lval* lenv_get(lenv* e, lval* var) {
  /* loop through each of variables, find the variable corresponding to a -- using strcmp -- then look up and return the corresponding lval, or else a copy?  Otherwise, return an error stating that the variable is not defined. */
  for (int i = 0; i < e->count; i++) {
    if (!strcmp(e->variables[i], var->op)) return lval_copy(e->assignments[i]);
  }
  // replace with macro to return specific name of undefined variable
  return lval_err("undefined symbol");
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin fn) {
    lval* var = lval_op(name);
    lval* asgn = lval_fn(fn);
    lenv_put(e, var, asgn);
    lval_del(var); lval_del(asgn);
}

void lenv_add_builtins(lenv* e) {
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_subtract);
    lenv_add_builtin(e, "/", builtin_divide);
    lenv_add_builtin(e, "*", builtin_multiply);
    lenv_add_builtin(e, "%", builtin_modulus);

    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "init", builtin_init);
    lenv_add_builtin(e, "len", builtin_len);
}