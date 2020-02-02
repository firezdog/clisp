#define LAMBDA(type_) ({\
    type_ _; \
})

#include<stdio.h>

// the problem
void invoke_callback(int range, int (*callback_for_int) (int)) 
{
    for (int i = 0; i < range; i++)
    {
        printf("%d\n", callback_for_int(i));
    }
}

void main()
{
    // invoke_callback(
    //     10,
    //     ({
    //         void callback(int n) 
    //         { 
    //             printf("%d\n", n); 
    //         } 
    //         callback; 
    //     })
    // );

    invoke_callback(10, LAMBDA(int _(int n) { printf("%d\n", n); return n; }));
}