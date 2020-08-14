#include <stdio.h>
#include <stdlib.h>
#include "../mpc_library/mpc.h"

#if defined(__CYGWIN__) | defined(_WIN32)
#include <string.h>

static char buffer[2048];

/* Fake readline function -- displays a prompt (passed), gets user input, stores it in a buffer, copies contents of the buffer, and returns */
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    // malloc = memory allocation for cpy variable to be as large as the buffer + 1
    char* cpy = malloc(strlen(buffer)+1);
    // copy buffer to cpy
    strcpy(cpy, buffer);
    // format cpy to end with 0 (end of string character?)
    cpy[strlen(cpy)-1] = '\0';
    // return string
    return cpy;
}

/* Fake add_history function -- not needed in windows implementation */
void add_history(char* unused) {}

#else
// compile with flag "-ledit"
// #include <editline/history.h> // not needed in OSX
#include <editline/readline.h>

#endif

int eval_op(int a, char* op, int b) {
    // using ! for falsy 0 (unintitive and prob less legible but more concise)
    if (!strcmp(op, "+")) { return a + b; }
    if (!strcmp(op, "-")) { return a - b; }
    if (!strcmp(op, "*")) { return a * b; }
    // c language doesn't have exceptions, so I don't know how to handle case where b is 0
    if (!strcmp(op, "/")) { return a / b; }
    return 0;
}

int evaluate(mpc_ast_t* t) {
    // apparently 0 is also "falsy" in C?
    if (strstr(t->tag, "numeral")) {
        return atoi(t->contents);
    } // else un-necessary b/c of return statement above
    
    /* first node is an expr? -- 2nd child of expr is always op */
    char* op = t->children[1]->contents;

    /* left expression */
    int x = evaluate(t->children[2]);

    /* go until you reach ')'?  -- I just don't find this intuitive -- I guess it allows for any number of arguments to flank an operator */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, evaluate(t->children[i]));
        i++;
    }

    return x;
}

int countBranches(mpc_ast_t* t) {
    int branches = t->children_num;
    for (int i = 0; i < t->children_num ; i++) {
        branches += countBranches(t->children[i]);
    }
    return branches;
}

void traverse(mpc_ast_t* t) {
    int size = countBranches(t) + 1;
    mpc_ast_t* queue[size];
    for (int i = 0; i < size; i++) {
        queue[i] = NULL;
    }
    queue[0] = t;
    while (queue[0]) {
        mpc_ast_t* current = queue[0];
        printf("%s : %s\n", current->tag, current->contents);
        queue[0] = NULL;
        for (int i = 0; i < size-1; i++) {
            queue[i] = queue[i+1];
        }
        int j = 0;
        while (queue[j]) { j++; }
        for (int i = 0; i < current->children_num; i++) {
            queue[j] = current->children[i];
            j++;
        }
    }
}

int main(int argc, char** argv) {

    // define parsers
    mpc_parser_t* Numeral = mpc_new("numeral");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    // define grammar for parsers
    mpca_lang(MPCA_LANG_DEFAULT,
    "numeral: /-?[0-9]+/ ;\
    operator: '+' | '-' | '*' | '/' ;\
    expr: <numeral> | '(' <operator> <expr>+ ')' ;\
    lispy: /^/ <operator> <expr>+ /$/ ;",
    Numeral, Operator, Expr, Lispy);

    // system info
    puts("Lispy Version 4");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        // prompt for input and echo
        char* input = readline("lispy> ");
        add_history(input);

        // now we echo the grammar
        // used for parsing
        mpc_result_t r;    
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // Load AST from output
            mpc_ast_t* a = r.output;
            traverse(a);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.output);
            mpc_err_delete(r.output);
        }
        free(input);
    }

    // clean up
    mpc_cleanup(4, Numeral, Operator, Expr, Lispy);
    // done
    return 0;

}