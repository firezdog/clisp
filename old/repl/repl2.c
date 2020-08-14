// Using editline (for improved user experience -- install libedit-dev on Linux and link with -ledit)
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv) {
    /* Print Version and Exit Information */
    puts("Lispy Version 1.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("lispy> ");
        add_history(input);
        printf("echo %s\n", input);
        free(input);
    }
}