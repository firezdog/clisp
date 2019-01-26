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

typedef struct {
    int type;
    double num;
    int err;
} lval;

enum { LVAL_NUM, LVAL_ERR, LERR_NUM, LERR_OP, LERR_DIV_ZERO };

lval lval_num(double x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

void lval_print(lval v) {
    switch(v.type) {
        case(LVAL_NUM)  :   printf("%g", v.num);  break;
        case(LVAL_ERR)  :
            switch(v.err) {
                case(LERR_DIV_ZERO) :   printf("Error: divide by zero.");     break;
                case(LERR_NUM)      :   printf("Error: bad number.");         break;
                case(LERR_OP)       :   printf("Error: bad operation.");      break;
            }
        break;
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval eval_op(lval a, char* op, lval b) {
    // doesn't cover the case in which both are errors
    if (a.type == LVAL_ERR) { return a; }
    if (b.type == LVAL_ERR) { return b; }
    // using ! for falsy 0 (unintitive and prob less legible but more concise)
    if (!strcmp(op, "+")) { return lval_num(a.num + b.num); }
    if (!strcmp(op, "-")) { return lval_num(a.num - b.num); }
    if (!strcmp(op, "*")) { return lval_num(a.num * b.num); }
    // c language doesn't have exceptions, so I don't know how to handle case where b is 0
    if (!strcmp(op, "/") && b.num != 0) { return lval_num(a.num / b.num); }
    if (!strcmp(op, "/") && b.num == 0) { return lval_err(LERR_DIV_ZERO); }
    if (!strcmp(op, "%") && b.num != 0) { return lval_num((int) a.num % (int) b.num); }
    if (!strcmp(op, "%") && b.num == 0) { return lval_err(LERR_DIV_ZERO); }
}

lval evaluate(mpc_ast_t* t) {
    // apparently 0 is also "falsy" in C?
    if (strstr(t->tag, "numeral")) {
        errno = 0;
        double x = strtod(t->contents, NULL);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_NUM);
    } // else un-necessary b/c of return statement above
    
    /* first node is an expr? -- 2nd child of expr is always op */
    char* op = t->children[1]->contents;

    /* left expression */
    lval x = evaluate(t->children[2]);

    /* go until you reach ')'?  -- I just don't find this intuitive -- I guess it allows for any number of arguments to flank an operator */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, evaluate(t->children[i]));
        i++;
    }

    return x;
}

int main(int argc, char** argv) {

    // define parsers
    mpc_parser_t* Numeral = mpc_new("numeral");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    // define grammar for parsers
    mpca_lang(MPCA_LANG_DEFAULT,
    "numeral: /-?([0-9]*\\.)?[0-9]+/ ;\
    operator: '+' | '-' | '*' | '/' | '\%' ;\
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
            lval result = evaluate(a);
            lval_println(result);
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