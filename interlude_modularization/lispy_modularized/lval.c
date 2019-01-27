#include<stdio.h>
#include "lval.h"

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
                case(LERR_NUM)      :   printf("Error: bad number.");         break;
                case(LERR_OP)       :   printf("Error: bad operation.");      break;
            }
        break;
    }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }