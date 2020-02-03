#include "lispy.h"

int main(int argc, char** argv) {
    // new function eventually?
    init_grammar();
    lenv* e = lenv_new();
    lenv_add_builtins(e);
    run_lisp = 1;
    
    // prompt
    puts("Lispy");
    puts("Press Ctrl+c to Exit\n");
    
    while (run_lisp) {
        input = readline("lispy> ");
        add_history(input);
        parse(e);
        free(input);
    }

    puts("Goodbye!");
    lenv_del(e);
    cleanup();
    return 0;
}