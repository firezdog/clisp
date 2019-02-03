#include "lispy.h"

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