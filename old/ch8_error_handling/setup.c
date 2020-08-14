#include <stdio.h>

typedef struct {
    int type;
    int num;
    int err;
} lval;

// some types
enum { LVAL_NUM, LVAL_ERR };

// some errors
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// constructors
lval lval_num(int x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

void lval_print(lval v) {
    switch(v.type) {
        case(LVAL_NUM)  :   printf("%i", v.num);  break;
        case(LVAL_ERR)  :
            switch(v.err) {
                case(LERR_DIV_ZERO) :   printf("Error: divide by zero.");     break;
                case(LERR_BAD_NUM)  :   printf("Error: bad number.");         break;
                case(LERR_BAD_OP)   :   printf("Error: bad operation.");      break;
            }
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

int main() {
    lval number = lval_num(10);
    lval zero = lval_err(LERR_DIV_ZERO);
    lval bad_num = lval_err(LERR_BAD_NUM);
    lval bad_op = lval_err(LERR_BAD_OP);
    lval_println(number);
    lval_println(zero);
    lval_println(bad_num);
    lval_println(bad_op);
    return 0;
}