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
// #include <editline/history.h> // not needed in MacOS X
#include <editline/readline.h>

#endif

void print_info(mpc_ast_t* t) {
    printf("Tag: %s\n", t->tag);
    printf("Contents: %s\n", t->contents);
    printf("Number of children: %i\n", t->children_num);
    printf("=============================\n");
}

int count_nodes(mpc_ast_t* t) {
    if (t->children_num == 0) {
        print_info(t);
        return 1; 
    }
    else {
        int total = 1;
        for (int i = 0; i < t->children_num; i++) {
            print_info(t->children[i]);
            total += count_nodes(t->children[i]);
        }
        return total;
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
            printf("Total number of nodes: %i\n", count_nodes(a));
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