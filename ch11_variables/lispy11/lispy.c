#include "lispy.h"

int main(int argc, char** argv) {
    // new function eventually?
    mpc_parser_t* Lispy = init_grammar();
    lenv* e = lenv_new();
    lenv_add_builtins(e);
    
    // prompt
    puts("Lispy");
    puts("Press Ctrl+c to Exit\n");
    
    while (1) {
        input = readline("lispy> ");
        add_history(input);
        parse(e);
        free(input);
    }

    lenv_del(e);
    cleanup();
    return 0;
}