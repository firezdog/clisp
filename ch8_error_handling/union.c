#include <stdio.h>
#include <string.h>

typedef union {
    float f;
    int i;
    char str[256]; 
} Data;

int main() {
    Data data;
    strcpy(data.str, "Hello world!");
    data.i = 3000;
    data.f = 3.14;
    // you cannot use multiple variables at same time -- memory gets overwritten (?) and corrupted
    printf("%f\n", data.f);
    printf("%s\n", data.str);
    printf("%i\n", data.i);
    return 0;
}