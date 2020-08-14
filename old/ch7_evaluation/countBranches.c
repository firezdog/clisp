// ../mpc_library/mpc.c and ledit flags needed to compile

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
// #include <editline/history.h>
#include <editline/readline.h>

#endif

// problem diagnosis -- I fell for the oldest pitfall in the book: I was adding to the variable I iterated over
int countBranches(mpc_ast_t* t) {
    int branches = t->children_num;
    for (int i = 0; i < t->children_num ; i++) {
        branches += countBranches(t->children[i]);
    }
    return branches;
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
            printf("%i\n", countBranches(a));
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