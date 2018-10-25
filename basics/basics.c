#include <stdio.h>

// Seems like the function definitions have to come first.

void forHelloFiveTimes() {
    for (int i = 0; i < 5; i++) {
        puts("Hello, world!");
    }
}

void whileHelloFiveTimes() {
    int i = 0;
    while (i < 5) {
        puts("Hello, world!");
        i++;
    }
}

void stringNTimes(int n, char* string) {
    for (int i = 0; i < n; i++) {
        puts(string);
    }
}

int main(int argc, char** argv){
    stringNTimes(5, "Goodbye!");
}

// built-in types: void, int, long, float, double, boolean, signed/unsigned (+/-), short (i.e. short int), bool (under <stdbool.h>), others?
// operators: +, -, *, /, %, ++, --, ==, !=, <, <=, >, >=, &&, ||, !, =, +=, -=, etc. some advanced stuff i don't need to list

// do...while
/* do  {
    ...;
} while (...); */

/* switch(variable) {
    case 1 :
        ...;
        break;
    case 2 :
        ...;
        break;
    default :
        ...;
} */

// *break* out of a loop or *continue* to the next iteration of the loop

// typedef -- defines a new type