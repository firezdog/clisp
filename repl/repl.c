//The Read-Evaluate-Print-Loop (REPL)
#include<stdio.h>

//store input of up to 2048 characters in length
static char input[2048];

int main(int argc, char** argv) {
    
    //Info
    puts("Lispy Version 1");
    puts("Press Ctrl + c to exit");

    while (1) {
        // put to and get from the console (stdout or stdin)
        fputs("lispy> ", stdout);
        fgets(input, 2048, stdin);
        printf("echo %s", input);
    }

    return 0;
}