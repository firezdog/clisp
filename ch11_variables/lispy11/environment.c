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

void lenv_put(lenv* e, lval* a) {
    /* puts a new value in the environment dictionary.  First, loop through and check if it is in the dictionary.  If so, delete the corresponding value and put the new one in.  If not, add it to the variables list, then add the corresponding assignment. 
    
    Note: here we put a copy in rather than the processed input itself -- presumably because the input is always getting deleted after the end of processing?
    */
    for (int i = 0; i < e->count; i++) {
        if (!strcmp(e->assignments[i], a->op)) { 
            lval_del(e->assignments[i]);
            e->assignments[i] = lval_copy(a);
            return; 
        }
    }
    int n = ++e->count;
    e->variables = realloc(e->variables, sizeof(char*) * n);
    e->assignments = realloc(e->assignments, sizeof(lval*) * n);
    e->variables[n - 1] = malloc(strlen(a->op) + 1);
    strcpy(e->variables[n - 1], a->op);
    e->assignments[n - 1] = lval_copy(a);
}

lval* lenv_get(lenv* e, char* a) {
  /* loop through each of variables, find the variable corresponding to a -- using strcmp -- then look up and return the corresponding lval, or else a copy?  Otherwise, return an error stating that the variable is not defined. */
  for (int i = 0; i < e->count; i++) {
    if (!strcmp(e->variables[i], a)) return lval_copy(e->assignments[i]);
  }
  // not sure if this will work as intended with # symbol
  return lval_err("The variable #a is not defined.");
}