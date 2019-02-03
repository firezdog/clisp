#include "lispy.h"

void parse() {
    mpc_result_t r;    
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
        // Load AST from output
        mpc_ast_t* a = r.output;
        lval* analysis = lval_read(r.output);
        // analysis is deleted in the course of lval_eval
        lval* result = lval_eval(analysis);
        lval_println(result);
        lval_del(result);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.output);
        mpc_err_delete(r.output);
    }
}

int main(int argc, char** argv) {
    init_grammar();
    // system info
    
    puts("Lispy Version 4");
    puts("Press Ctrl+c to Exit\n");
    
    while (1) {
        input = readline("lispy> ");
        add_history(input);
        parse();
        free(input);
    }
    
    cleanup();
    return 0;
}