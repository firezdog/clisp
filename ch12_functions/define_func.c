#include<stdio.h>

#define lambda(type, body) \
({ \
    type fn body \
        fn; \
})

void main() {
    void (*say_hello)(char*) = lambda(void, (char* str) { printf("Hello %s\n", str); });
    say_hello("world");
}