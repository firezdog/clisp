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
#include <editline/history.h>
#include <editline/readline.h>

#endif

int evaluate(mpc_ast_t* t) {
    if (strstr(t->tag, "numeral") != 0) {
        return atoi(t->contents);
    } else {
        // seems like the loops is needed here when working your way in to terms on the right.  Perhaps the loop could be replaced with an if check.
        // you need to evaluate both the left and the right terms :p
        // Operations must be flanked by two terms -- I consider operations flanked by more than two terms to yield ill-formed expressions.
        // Note that this just drills down whatever the first correctly formed expression is and evaluates that.  "- (+ 3 5)" is technically not a complete expression, though it is well-formed (according to the syntactical rules); intuitively it would evaluate to -8 but it in fact evaluates to 8 -- the incomplete expression apparently never gets evaluated... (actually, intuition might not be correct -- this might be treated as "- (+ 3 5) 0")
        for (int i = 0; i < t->children_num; i++) {
            if (strcmp(t->children[i]->contents,"+") == 0) {
                return evaluate(t->children[i+1]) + evaluate(t->children[i+2]);
            } else if (strcmp(t->children[i]->contents, "-") == 0) {
                return evaluate(t->children[i+1]) - evaluate(t->children[i+2]);
            } else if (strcmp(t->children[i]->contents, "*") == 0) {
                return evaluate(t->children[i+1]) * evaluate(t->children[i+2]);
            } else if (strcmp(t->children[i]->contents, "/") == 0) {
                return evaluate(t->children[i+1]) / evaluate(t->children[i+2]);
            }
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
            printf("%i\n", evaluate(a));
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