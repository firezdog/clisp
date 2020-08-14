#include <stdio.h>

enum week {Monday, Tuesday, Wednesday, Thursday, Friday};
enum week day = Monday;

void print_day(enum week day) {
    switch (day) {
        case(Monday): printf("It's Monday!\n"); break;
        case(Tuesday): printf("It's Tuesday!\n"); break;  
        case(Wednesday): printf("It's Wednesday!\n"); break;
        case(Thursday): printf("It's Thursday!\n"); break;
        case(Friday): printf("It's Friday!\n"); break;
    }
}

int main() {
    print_day(Monday);
    print_day(Tuesday);
    print_day(Wednesday);
    print_day(Thursday);
    print_day(Friday);
    return 0;
}