#include "lispy.h"

lenv* lenv_new(void) {
    lenv* v = malloc(sizeof(lenv));
    v->parent_environment = NULL;
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

lenv* lenv_copy(lenv* e) {
    lenv* c = lenv_new();
    c->parent_environment = e->parent_environment;
    c->count = e->count;
    c->variables = malloc(sizeof(char*) * c->count);
    c->assignments = malloc(sizeof(lval*) * c->count);
    for (int i = 0; i < c->count; i++) {
        c->variables[i] = malloc(strlen(e->variables[i])+1);
        strcpy(c->variables[i], e->variables[i]);
        c->assignments[i] = lval_copy(e->assignments[i]);
    }
    return c;
}

void lenv_put(lenv* e, lval* var, lval* asgn) {
    /* puts a new value in an environment dictionary. Note: here we put a copy in rather than the processed input itself -- presumably because the input is always getting deleted after the end of processing */
   // A. If the variable is already in the env, replace its assignment with the new value
    for (int i = 0; i < e->count; i++) {
        if (!strcmp(e->variables[i], var->op)) { 
            lval_del(e->assignments[i]);
            e->assignments[i] = lval_copy(asgn);
            return; 
        }
    }
    // B. Otherwise, add it and its assignment to the environment.
    int n = ++e->count;
    e->variables = realloc(e->variables, sizeof(char*) * n);
    e->assignments = realloc(e->assignments, sizeof(lval*) * n);
    e->variables[n - 1] = malloc(strlen(var->op) + 1);
    strcpy(e->variables[n - 1], var->op);
    e->assignments[n - 1] = lval_copy(asgn);
}

void root_lenv_put(lenv* e, lval* var, lval* asgn) {
    while (e->parent_environment) {
        e = e->parent_environment;
    }
    lenv_put(e, var, asgn);
}

lval* lenv_get(lenv* e, lval* var) {
  /* loop through each of variables, find the variable corresponding to a -- using strcmp -- then look up and return the corresponding lval, or else a copy?  Otherwise, return an error stating that the variable is not defined. */
  // break the recursion
  if (!e) return lval_err("undefined symbol: %s", var->op);
  for (int i = 0; i < e->count; i++) {
    if (!strcmp(e->variables[i], var->op)) {
        return lval_copy(e->assignments[i]);
    }
  }
  return lenv_get(e->parent_environment, var);
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

    // logic and comparison
    lenv_add_builtin(e, ">", builtin_greater);
    lenv_add_builtin(e, "<", builtin_lesser);
    lenv_add_builtin(e, "or", builtin_or);
    lenv_add_builtin(e, "and", builtin_and);
    lenv_add_builtin(e, "!", builtin_not);
    lenv_add_builtin(e, "=", builtin_equals);
    lenv_add_builtin(e, "?", builtin_ternary);

    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "init", builtin_init);
    lenv_add_builtin(e, "len", builtin_len);

    lenv_add_builtin(e, "env", builtin_print_env);
    lenv_add_builtin(e, "exit", builtin_exit);

    // the most important of the builtins!
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "let", builtin_let);
    lenv_add_builtin(e, "\\", builtin_lambda);
}